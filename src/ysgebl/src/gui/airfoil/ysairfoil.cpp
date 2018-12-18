/* ////////////////////////////////////////////////////////////

File Name: ysairfoil.cpp
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

// http://en.wikipedia.org/wiki/NACA_airfoil

#include <stdio.h>
#include <math.h>

#include <ysclass.h>
#include "ysairfoil.h"


void Airfoil::ResampleFromUpperAndLower(const YsArray <YsVec3> &upper,const YsArray <YsVec3> &lower,const YsArray <double> &tArray)
{
	point.CleanUp();

	frontIdx=0;

	YsTraceLineSegment lseg;
	lseg.SetLineSegment(upper.GetN(),upper,YSFALSE);

	YsVec2 pos(upper[0].x(),upper[0].y());
	point.Append(pos);

	for(int i=1; i<tArray.GetN(); ++i)
	{
		lseg.SetPositionByParameter(tArray[i]);

		const YsVec3 curPos=lseg.GetCurrentPosition();
		pos.Set(curPos.x(),curPos.y());

		point.Append(pos);
	}

	backIdx=(int)point.GetN()-1;

	if(upper.GetEnd()!=lower.GetEnd())
	{
		pos.Set(lower.GetEnd().x(),lower.GetEnd().y());
		point.Append(pos);
	}

	lseg.SetLineSegment(lower.GetN(),lower,YSFALSE);

	for(auto i=tArray.GetN()-2; 0<i; --i)
	{
		lseg.SetPositionByParameter(tArray[i]);

		const YsVec3 curPos=lseg.GetCurrentPosition();
		pos.Set(curPos.x(),curPos.y());

		point.Append(pos);
	}
}

void Airfoil::MatchTail(void)
{
	const double xMax=point[backIdx].x();

	const double rotAngle=atan2(-point[backIdx].y(),point[backIdx].x());
	for(int idx=0; idx<point.GetN(); ++idx)
	{
		point[idx].Rotate(rotAngle);
	}

	const double scale=xMax/point[backIdx].x();
	for(int idx=0; idx<point.GetN(); ++idx)
	{
		point[idx]=point[idx]*scale;
	}
}

void Airfoil::Normalize(void)
{
	double minX=0.0;
	double scale=fabs(point[backIdx].x()-point[frontIdx].x());
	for(int idx=0; idx<point.GetN(); ++idx)
	{
		point[idx]/=scale;
		if(0==idx)
		{
			minX=point[idx].x();
		}
		else
		{
			minX=YsSmaller(minX,point[idx].x());
		}
	}
	for(int idx=0; idx<point.GetN(); ++idx)
	{
		point[idx].SubX(minX);
	}
}

////////////////////////////////////////////////////////////

void AirfoilNACA::GenerateRawAirfoil(int naca,const double chordLength,int numDivision,YSBOOL sharpTail)
{
	upper.CleanUp();
	lower.CleanUp();

	const double c=chordLength;

	for(int i=0; i<=numDivision; ++i)
	{
		const double x=c*(double)i/(double)numDivision;
		upper.Append(YsVec3(x,YCoord(naca,c,x, 1.0,sharpTail),0.0));
	}

	for(int i=0; i<=numDivision; ++i)
	{
		const double x=c*(double)i/(double)numDivision;
		lower.Append(YsVec3(x,YCoord(naca,c,x,-1.0,sharpTail),0.0));
	}
}

void AirfoilNACA::Resample(int numDivision)
{
	int ix=0,step=1;

	YsArray <double> tArray;
	for(int i=0; i<numDivision; ++i)
	{
		tArray.Append((double)ix);
		ix+=step;

		if(1<=i)
		{
			++step;
		}
		if(2<=i)
		{
			++step;
		}
		if(numDivision/2<i)
		{
			++step;
		}
		if(numDivision*2/3<i)
		{
			++step;
		}
		if(numDivision*5/6<i)
		{
			++step;
		}
	}

	for(int i=0; i<tArray.GetN(); ++i)
	{
		tArray[i]/=tArray.GetEnd();
	}

	ResampleFromUpperAndLower(upper,lower,tArray);
}

double AirfoilNACA::YCoord(int naca,double c,double x,double side,YSBOOL sharpTail)
{
	const double t=(double)(naca%100)/100.0;

	const double E=(YSTRUE==sharpTail ? 0.1036 : 0.1015);
	// Original NACA specification gives -0.1015 nx^4 term, but it doesn't close 
	// the airfoil.  Modified coefficient is 0.1036, which will close the airfoil

	double y,nx;
	nx=x/c;
	y=(t*c/0.2)*(0.2969*sqrt(nx)-0.1260*(nx)-0.3516*nx*nx+0.2843*nx*nx*nx-E*nx*nx*nx*nx);

	const double m=(double)((naca/1000)%10)/100.0;
	const double p=(double)((naca/100)%10)/10.0;

	double yc;
	if(x<p*c)
	{
		yc=m*(x/(p*p))*(2.0*p-nx);
	}
	else
	{
		yc=(m*(c-x)/((1.0-p)*(1.0-p)))*(1.0+nx-2.0*p);
	}

	return yc+y*side;
}

////////////////////////////////////////////////////////////

void AirfoilJoukowski::Generate(int numDivision,const double a,const double d,const double e)
{
	YsArray <YsVec3> upper,lower;

	for(int ang=180; ang>=0; --ang)
	{
		double x,y;
		const double theata=(double)ang*YsPi/180.0;
		GetCoordinate(x,y,a,d,e,theata);
		upper.Append(YsVec3(x,y,0.0));
	}
	for(int ang=180; ang<=360; ++ang)
	{
		double x,y;
		const double theata=(double)ang*YsPi/180.0;
		GetCoordinate(x,y,a,d,e,theata);
		lower.Append(YsVec3(x,y,0.0));
	}

	int ix=0,step=1;

	YsArray <double> tArray;
	for(int i=0; i<numDivision; ++i)
	{
		tArray.Append((double)ix);
		ix+=step;

		if(i<numDivision/2)
		{
			if(1<=i)
			{
				++step;
			}
			if(2<=i)
			{
				++step;
			}
		}
		else if(numDivision*2/3<i && 1<step)
		{
			--step;
		}
	}

	for(int i=0; i<tArray.GetN(); ++i)
	{
		tArray[i]/=tArray.GetEnd();
	}

	ResampleFromUpperAndLower(upper,lower,tArray);
	Normalize();
}

void AirfoilJoukowski::GetCoordinate(double &x,double &y,const double a,const double d,const double e,const double theata)
{
	const double s=atan(d/e);

	const double A=1.0;
	const double B=2.0*sqrt(d*d+e*e)*cos(theata+s);
	const double C=-d*d-YsSqr(a+e)+d*d+e*e;

	const double r=(-B+sqrt(B*B-4.0*A*C))/(2.0*A);

	x=(r+a*a/r)*cos(theata);
	y=(r-a*a/r)*sin(theata);
}
