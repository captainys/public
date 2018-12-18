/* ////////////////////////////////////////////////////////////

File Name: ysadvgeometry.cpp
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
#include <string.h>
#include <stdlib.h>


#include "ysadvgeometry.h"
#include "yswizard.h"
#include "ysmalloc.h"
#include "ysequation.h"
#include "yspredefined.h"
#include "ysmergesort.h"
#include "ysavltree.h"


////////////////////////////////////////////////////////////

YSBOOL YsCheckConvexByAngle2(YSSIZE_T np,const YsVec2 *p)
{
	int i;
	const YsVec2 *p1,*p2;
	YsVec2 v1,v2;
	double o1=0.0,o2;

	while(np>3 && p[0]==p[1])
	{
		p++;
		np--;
	}
	while(np>3 && p[np-1]==p[np-2])
	{
		np--;
	}
	if(np>3 && p[np-1]==p[0])
	{
		np--;
	}

	if(np<=3)
	{
		return YSTRUE;
	}

 	p1=&p[np-2];
	p2=&p[np-1];
	v2=*p2-*p1;
	for(i=0; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(*p1!=*p2)
		{
			v1=v2;
			v2=*p2-*p1;

			o1=v1^v2;
			if(fabs(o1)>YsTolerance)
			{
				break;
			}
		}
	}

	for(i=i+1; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(*p1!=*p2)
		{
			v1=v2;
			v2=*p2-*p1;

			o2=v1^v2;
			if(YsAbs(o2)>YsTolerance && o1*o2<0)
			{
				return YSFALSE;
			}
		}
	}
	return YSTRUE;
}

YSBOOL YsCheckConvexByAngle2(const YsConstArrayMask <YsVec2> &plg)
{
	return YsCheckConvexByAngle2(plg.GetN(),plg);
}

YSBOOL YsCheckConvexByAngle3(YSSIZE_T np,const YsVec3 *p)
{
	int i;
	const YsVec3 *p1,*p2;
	YsVec3 v1,v2,o1,o2;

	while(np>3 && p[0]==p[1])
	{
		p++;
		np--;
	}
	while(np>3 && p[np-1]==p[np-2])
	{
		np--;
	}
	if(np>3 && p[np-1]==p[0])
	{
		np--;
	}

	if(np<=3)
	{
		return YSTRUE;
	}

 	p1=&p[np-2];
	p2=&p[np-1];
	v2=*p2-*p1;
	for(i=0; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(*p1!=*p2)
		{
			v1=v2;
			v2=*p2-*p1;

			o1=v1^v2;
			if(o1.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
			{
				break;
			}
		}
	}

	for(i=i+1; i<np; i++)
	{
		p1=p2;
		p2=&p[i];
		if(*p1!=*p2)
		{
			v1=v2;
			v2=*p2-*p1;

			o2=v1^v2;
			if(o2.GetSquareLength()>YsSqr(YsTolerance) && o1*o2<0)   // Modified 2000/11/10
			{
				return YSFALSE;
			}
		}
	}

	return YSTRUE;
}

YSBOOL YsCheckConvexByAngle3(const YsConstArrayMask <YsVec3> &plg)
{
	return YsCheckConvexByAngle3(plg.GetN(),plg);
}

YSBOOL YsCheckConvex2(YSSIZE_T np,const YsVec2 p[],YSBOOL strictCheck)
{
	if(strictCheck!=YSTRUE)
	{
		return YsCheckConvexByAngle2(np,p);
	}
	else
	{
		int i,j;

		if(np<=3)
		{
			return YSTRUE;
		}

		if(YsCheckConvexByAngle2(np,p)==YSTRUE)
		{
			for(i=0; i<np; i++)
			{
				double z;
				YSSIZE_T idx0,idx1,idx2;
				YsVec2 v1,v2;

				idx0=(i+np-1)%np;
				idx1=i;
				idx2=(i+1)%np;

				v1=p[idx1]-p[idx0];
				v2=p[idx2]-p[idx1];
				if(YSOK!=v1.Normalize() || YSOK!=v2.Normalize())
				{
					continue;
				}

				z=v1^v2;
				if(fabs(z)<YsTolerance)    // Modified 2000/11/10
				{
					continue;
				}

				for(j=0; j<np; j++)
				{
					if( i==j || // 2010/06/24
					    j==i-1 ||
					    j==i+1 ||
					   (i==0 && j==np-1) ||
					   (i==np-1 && j==0))
					{
						continue;      // Skip neighbor
					}

					idx0=(j+np-1)%np;
					idx1=j;
					idx2=(j+1)%np;

					v1=p[idx1]-p[idx0];
					v2=p[idx2]-p[idx1];
					if(YSOK!=v1.Normalize() || YSOK!=v2.Normalize())
					{
						continue;
					}

					z=v1^v2;
					if(fabs(z)<YsTolerance)   // Modified 2000/11/10
					{
						continue;     // Skip Straight lines case
					}

					if(YsCheckSeparatability2(np,p,i,j)!=YSTRUE)
					{
						goto CONCAVE;
					}
				}
			}
			return YSTRUE;
		}
	CONCAVE:
		return YSFALSE;
	}
}

YSBOOL YsCheckConvex2(const YsConstArrayMask <YsVec2> &plg,YSBOOL strictCheck)
{
	return YsCheckConvex2(plg.GetN(),plg,strictCheck);
}

YSBOOL YsCheckConvex3(YSSIZE_T np,const YsVec3 p[],YSBOOL strictCheck)
{
	if(strictCheck!=YSTRUE)
	{
		return YsCheckConvexByAngle3(np,p);
	}
	else
	{
		int i,j;

		if(np<=3)
		{
			return YSTRUE;
		}

		if(YsCheckConvexByAngle3(np,p)==YSTRUE)
		{
			for(i=0; i<np; i++)
			{
				YsVec3 z,v1,v2;
				YSSIZE_T idx0,idx1,idx2;

				idx0=(i+np-1)%np;
				idx1=i;
				idx2=(i+1)%np;

				v1=p[idx1]-p[idx0];
				v2=p[idx2]-p[idx1];
				if(YSOK!=v1.Normalize() || YSOK!=v2.Normalize())
				{
					continue;
				}

				z=v2^v1;
				if(z.GetSquareLength()<YsSqr(YsTolerance))  // Modified 2000/11/10
				{
					continue;
				}

				for(j=0; j<np; j++)
				{
					if( i==j || // 2010/06/24
					    j==i-1 ||
					    j==i+1 ||
					   (i==0 && j==np-1) ||
					   (i==np-1 && j==0))
					{
						continue;      // Skip neighbor
					}

					idx0=(j+np-1)%np;
					idx1=j;
					idx2=(j+1)%np;

					v1=p[idx1]-p[idx0];
					v2=p[idx2]-p[idx1];
					if(YSOK!=v1.Normalize() || YSOK!=v2.Normalize())
					{
						continue;
					}

					z=v2^v1;
					if(z.GetSquareLength()<YsSqr(YsTolerance))  // Modified 2000/11/10
					{
						continue;     // Skip Straight lines case
					}

					if(YsCheckSeparatability3(np,p,i,j)!=YSTRUE)
					{
						goto CONCAVE;
					}
				}
			}
			return YSTRUE;
		}
	CONCAVE:
		return YSFALSE;
	}
}

YSBOOL YsCheckConvex3(const YsConstArrayMask <YsVec3> &plg,YSBOOL strictCheck)
{
	return YsCheckConvex3(plg.GetN(),plg,strictCheck);
}

////////////////////////////////////////////////////////////

double YsGetTriangleArea3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3)
{
	YsVec3 a,b;

	a=p2-p1;
	b=p3-p1;

//	double c,s,la,lb;
//	la=a.GetLength();
//	lb=b.GetLength();
//	c=a*b/(la*lb);
//	s=sqrt(1.0-c*c);
//	return la*lb*s/2.0;

	return (a^b).GetLength()/2.0;
}

////////////////////////////////////////////////////////////

double YsGetTriangleArea2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3)
{
	YsVec2 a,b;
	a=p2-p1;
	b=p3-p1;
	return (a^b)/2.0;
}

////////////////////////////////////////////////////////////

double YsGetPolygonArea2(YSSIZE_T nPlVt,const YsVec2 p[])
{
	int i;
	double totalCrsPrd,crsPrd;
	YsVec2 o,p1,p2;

	if(nPlVt>=3)
	{
		totalCrsPrd=0.0;
		o=p[0];
		p2=p[1];
		for(i=1; i<nPlVt-1; i++)
		{
			p1=p2;
			p2=p[i+1];
			crsPrd=(p2-o)^(p1-o);
			totalCrsPrd+=crsPrd;
		}
		return totalCrsPrd/2.0;
	}
	return 0.0;
	
}

double YsGetPolygonArea2(const YsConstArrayMask <YsVec2> &plg)
{
	return YsGetPolygonArea2(plg.GetN(),plg);
}

////////////////////////////////////////////////////////////

double YsGetPolygonArea(YSSIZE_T np,const YsVec2 p[])
{
	return YsGetPolygonArea2(np,p);
}

double YsGetPolygonArea(const YsConstArrayMask <YsVec2> &plg)
{
	return YsGetPolygonArea2(plg);
}

double YsGetPolygonArea(YSSIZE_T np,const YsVec3 p[])
{
	auto areaVec=YsVec3::Origin();
	if(3<=np)
	{
		YsVec3 prevV=p[1]-p[0];
		for(YSSIZE_T idx=2; idx<np; ++idx)
		{
			YsVec3 v=p[idx]-p[0];
			areaVec+=(prevV^v);
			prevV=v;
		}
	}
	return areaVec.GetLength()/2.0;
}

double YsGetPolygonArea(const YsConstArrayMask <YsVec3> &plg)
{
	return YsGetPolygonArea(plg.size(),plg.data());
}

////////////////////////////////////////////////////////////

YSRESULT YsGetLargestTriangleFromPolygon3(const YsVec3 *v[3],YSSIZE_T np,const YsVec3 p[])
{
	int i,j,k;
	double candidate,current;
	if(np==3)
	{
		v[0]=&p[0];
		v[1]=&p[1];
		v[2]=&p[2];
		return YSOK;
	}
	else if(np>=3)
	{
		YsVec3 v1;
		candidate=0.0;
		v[0]=&p[0];
		v[1]=&p[1];
		v[2]=&p[2];
		for(i=0; i<np; i++)
		{
			for(j=i+1; j<np; j++)
			{
				v1=p[j]-p[i];            // 2009/03/31
				for(k=j+1; k<np; k++)
				{
					current=(v1^(p[k]-p[i])).GetSquareLength();
					if(current>candidate)
					{
						candidate=current;
						v[0]=&p[i];
						v[1]=&p[j];
						v[2]=&p[k];
					}
				}
			}
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGetLargestTriangleFromPolygon3(const YsVec3 *v[3],const YsConstArrayMask <YsVec3> &plg)
{
	return YsGetLargestTriangleFromPolygon3(v,plg.GetN(),plg);
}

YSRESULT YsGetLargestTriangleFromPolygon3(YsVec3 v[3],YSSIZE_T np,const YsVec3 p[])
{
	const YsVec3 *tri[3];
	if(YsGetLargestTriangleFromPolygon3(tri,np,p)==YSOK)
	{
		v[0]=*tri[0];
		v[1]=*tri[1];
		v[2]=*tri[2];
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGetLargestTriangleFromPolygon3(YsVec3 v[3],const YsConstArrayMask <YsVec3> &plg)
{
	return YsGetLargestTriangleFromPolygon3(v,plg.GetN(),plg);
}

////////////////////////////////////////////////////////////

YSRESULT YsGetLargestTriangleFromPolygon2(const YsVec2 *v[3],YSSIZE_T np,const YsVec2 p[])
{
	int i,j,k;
	double candidate,current;

	if(np==3)
	{
		v[0]=&p[0];
		v[1]=&p[1];
		v[2]=&p[2];
		return YSOK;
	}
	else if(np>3)
	{
		YsVec2 v1;

		candidate=0.0;
		v[0]=&p[0];
		v[1]=&p[1];
		v[2]=&p[2];
		for(i=0; i<np; i++)
		{
			for(j=i+1; j<np; j++)
			{
				v1=p[j]-p[i];
				for(k=j+1; k<np; k++)
				{
					current=fabs(v1^(p[k]-p[i]));
					if(current>candidate)
					{
						candidate=current;
						v[0]=&p[i];
						v[1]=&p[j];
						v[2]=&p[k];
					}
				}
			}
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGetLargestTriangleFromPolygon2(const YsVec2 *v[3],const YsConstArrayMask <YsVec2> &plg)
{
	return YsGetLargestTriangleFromPolygon2(v,plg.GetN(),plg);
}

YSRESULT YsGetLargestTriangleFromPolygon2(YsVec2 v[3],YSSIZE_T np,const YsVec2 p[])
{
	const YsVec2 *tri[3];
	if(YsGetLargestTriangleFromPolygon2(tri,np,p)==YSOK)
	{
		v[0]=*tri[0];
		v[1]=*tri[1];
		v[2]=*tri[2];
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGetLargestTriangleFromPolygon2(YsVec2 v[3],const YsConstArrayMask <YsVec2> &plg)
{
	return YsGetLargestTriangleFromPolygon2(v,plg.GetN(),plg);
}

////////////////////////////////////////////////////////////

YSRESULT YsGetAverageNormalVector(YsVec3 &nom,YSSIZE_T np,const YsVec3 p[])
{
	int i;
	static int idMap[]={0,1,2,0,1,2};
	YsVec3 v[2];
	YSRESULT nomTest[2];
	if(np==3)
	{
		v[1]=p[0]-p[2];
		nomTest[1]=v[1].Normalize();
		for(i=0; i<3; i++)
		{
			v[i&1]=p[idMap[i+1]]-p[idMap[i]];
			nomTest[i&1]=v[i&1].Normalize();
			if(nomTest[0]==YSOK && nomTest[1]==YSOK)    // Modified 2003/05/02
			{
				nom=v[1-(i&1)]^v[i&1];
				if(nom.Normalize()==YSOK)  // 2009/03/31 It will try next edge if the first one does not work.
				{
					return YSOK;
				}
				// return nom.Normalize();    // Modified 2003/05/02  Commented out 2009/03/31
			}
		}
	}
	else if(np>3)
	{
		const YsVec3 *tri[3];
		if(YsGetLargestTriangleFromPolygon3(tri,np,p)==YSOK)
		{
			v[1]=*tri[0]-*tri[2];
			nomTest[1]=v[1].Normalize();

			for(i=0; i<3; i++)
			{
				v[i&1]=(*tri[idMap[i+1]]-*tri[idMap[i]]);
				nomTest[i&1]=v[i&1].Normalize();
				if(nomTest[0]==YSOK && nomTest[1]==YSOK)    // Modified 2003/05/02
				{
					nom=v[1-(i&1)]^v[i&1];
					if(nom.Normalize()==YSOK)  // 2009/03/31 It will try next edge if the first one does not work.
					{
						return YSOK;
					}
					// return nom.Normalize();    // Modified 2003/05/02  Commented out 2009/03/31
				}
			}
		}
	}
	return YSERR;
}

YsVec3  YsGetAverageNormalVector(YSSIZE_T np,const YsVec3 p[])
{
	YsVec3 nom;
	YsGetAverageNormalVector(nom,np,p);
	return nom;
}


////////////////////////////////////////////////////////////


/************************************************************************

   1)y-y1=dy/dx*(x-x1)
   2)dx(y-y1)=dy(x-x1)
   3)dx*y-dx*y1=dy*x-dy*x1
   4)dy*x-dx*y-dy*x1+dx*y1=0

   ax+by+c=0
     a= dy
     b=-dx
     c=-dy*x1+dx*y1

   a1 x  +b1 y  +c1 = 0
   a2 x  +b2 y  +c2 = 0

   a2a1 x  +a2b1 y  +a2c1 = 0
   a1a2 x  +a1b2 y  +a1c2 = 0
  ----------------------------
         (a2b1-a1b2)y +a2c1-a1c2 = 0


   a1b2 x  +b1b2 y  +c1b2 = 0
   a2b1 x  +b2b1 y  +c2b1 = 0
  ----------------------------
   (a1b2-a2b1)x +c1b2-c2b1 = 0

************************************************************************/

/* Not used any longer.  Commented out 2010/04/18
static void YsGetLineCoeffs
    (double *a,double *b,double *c,const YsVec2 &o,const YsVec2 &v)
{
	*a= v.y();
	*b=-v.x();
	*c=-v.y()*o.x() +v.x()*o.y();
} */

YSRESULT YsGetLineIntersection2
  (YsVec2 &crs,
   const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &q1,const YsVec2 &q2)
{
//   q=o1+s*v1
//   q=o2+t*v2   ->  o1+s*v1=o2+t*v2  ->  s*v1-t*v2=o2-o1

//  Matrix form
//
//  (v1x  -v2x)(s) = (o2x-o1x)
//  (v1y  -v2y)(t)   (o2y-o1y)

	const YsVec2 &o1=p1,&o2=q1;
	YsVec2 o1o2;
	YsVec2 v1,v2;

	o1o2=o2-o1;
	v1=p2-p1;
	v2=q2-q1;

	double inv[4],mat[4];
	mat[0]=v1.x();   mat[1]=-v2.x();
	mat[2]=v1.y();   mat[3]=-v2.y();

	if(YsInvertMatrix2x2(inv,mat)==YSOK)
	{
		double s,t;
		s=inv[0]*o1o2.x()+inv[1]*o1o2.y();
		t=inv[2]*o1o2.x()+inv[3]*o1o2.y();
		if(YsAbs(s)>YsAbs(t))
		{
			crs=o1+s*v1;
		}
		else
		{
			crs=o2+t*v2;
		}
		return YSOK;
	}
	return YSERR;


/* Commented out 04/21/2001
	double a1,b1,c1;
	double a2,b2,c2;
	double p,q;
	double x,y;
	YsVec2 vp,vq;

	vp=p2-p1;
	vq=q2-q1;

	YsGetLineCoeffs(&a1,&b1,&c1,p1,vp);
	YsGetLineCoeffs(&a2,&b2,&c2,q1,vq);

	p=a2*b1-a1*b2;
	q=a2*c1-a1*c2;
	if(YsEqual(p,0.0)==YSTRUE)
	{
		return YSERR;
	}
	y=-q/p;

	p=a1*b2-a2*b1;
	q=c1*b2-c2*b1;
	if(YsEqual(p,0.0)==YSTRUE)
	{
		return YSERR;
	}
	x=-q/p;

	crs.Set(x,y);
	return YSOK;
*/
}

YSBOOL YsCheckLineOverlap2
    (const YsVec2 &o1,const YsVec2 &v1,const YsVec2 &o2,const YsVec2 &v2)
{
	YsVec2 nv1,nv2,nvo1o2;

	nv1=v1;
	nv2=v2;
	nvo1o2=o2-o1;

	if(nv1.Normalize()==YSOK && nv2.Normalize()==YSOK)
	{
		if(o1==o2)
		{
			return YSTRUE;
		}
		else if(nvo1o2.Normalize()==YSOK &&
			    YsAbs(nv1^nv2)<YsTolerance &&
			    YsAbs(nvo1o2^nv2)<YsTolerance)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;

// Commented out 2001/04/21
/*
	if(YsAbs(v1^v2)<YsTolerance &&
	   YsAbs((o2-o1)^v1)<YsTolerance &&
	   v1.GetSquareLength()>YsSqr(YsTolerance) &&
	   v2.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		return YSTRUE;
	}
	return YSFALSE;
*/

/* Comparing Coefficient method
	double a1,b1,c1;
	double a2,b2,c2;
	double p,q;

	YsGetLineCoeffs(&a1,&b1,&c1,o1,v1);
	YsGetLineCoeffs(&a2,&b2,&c2,o2,v2);

	p=a2*b1-a1*b2;
	q=a2*c1-a1*c2;
	if(YsEqual(p,0.0)==YSTRUE && YsEqual(q,0.0)==YSTRUE)
	{
		return YSTRUE;
	}

	p=a1*b2-a2*b1;
	q=c1*b2-c2*b1;
	if(YsEqual(p,0.0)==YSTRUE && YsEqual(q,0.0)==YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
*/
}

YSBOOL YsCheckLineOverlap3
    (const YsVec3 &o1,const YsVec3 &v1,const YsVec3 &o2,const YsVec3 &v2)
{
	YsVec3 nv1,nv2,nvo1o2;

	nv1=v1;
	nv2=v2;
	nvo1o2=o2-o1;

	if(nv1.Normalize()==YSOK && nv2.Normalize()==YSOK)
	{
		if(o1==o2)
		{
			return YSTRUE;
		}
		else if(nvo1o2.Normalize()==YSOK &&
			    (nv1^nv2).GetSquareLength()<YsSqr(YsTolerance) &&
			    (nvo1o2^nv2).GetSquareLength()<YsSqr(YsTolerance))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;

// Commented out 2001/04/21
/*
	if((v1^v2).GetSquareLength()<YsSqr(YsTolerance) &&
	   ((o2-o1)^v1).GetSquareLength()<YsSqr(YsTolerance) &&
	   v1.GetSquareLength()>YsSqr(YsTolerance) &&
	   v2.GetSquareLength()>YsSqr(YsTolerance))   // Modified 2000/11/10
	{
		return YSTRUE;
	}
	return YSFALSE;
*/
}



////////////////////////////////////////////////////////////

YSSIDE YsCheckInsidePolygon2(const YsVec2 &pnt,YSSIZE_T np,const YsVec2 p[])
{
	if(np==3)                                    // 2009/03/30
	{                                            // 2009/03/30
		return YsCheckInsideTriangle2(pnt,p);    // 2009/03/30
	}                                            // 2009/03/30


	int i,j,k,CrossCounter,vt1,vt2;
	double ang,edSqLen;
	YsVec2 vec,crs,cVec;
	const YsVec2 *p1,*p2,*o1,*o2;
	YsVec2 v1,v2;

	const int maxNumEdgSortTable=4;
	int nEdgSortTable;
	int edgSortIdx[maxNumEdgSortTable];
	double edgSortLng[maxNumEdgSortTable];

	//for(i=0; i<np; i++)
	//{
	//	printf("V %lf %lf 0.0\n",p[i].x(),p[i].y());
	//}
	//printf("V %lf %lf 0.0\n",pnt.x(),pnt.y());



	// Bounding Box Check added on 01/25/2000  Moved to upfront 2009/03/30 >>
	YsBoundingBoxMaker2 bbx;
	YsVec2 bbxMin,bbxMax;
	bbx.Begin();
	for(i=0; i<np; i++)
	{
		bbx.Add(p[i]);
		if(pnt==p[i])
		{
			return YSBOUNDARY;
		}
	}
	bbx.Get(bbxMin,bbxMax);
	if(YsCheckInsideBoundingBox2(pnt,bbxMin,bbxMax)!=YSTRUE)
	{
		return YSOUTSIDE;
	}
	// Bounding Box Check added on 01/25/2000  Moved to upfront 2009/03/30 <<



	nEdgSortTable=0;
	double avgLng;
	int edId; // 2009/03/30
	const YsVec2 *prv;

	edId=(int)(np-1);
	prv=&(p[np-1]);

	avgLng=0.0;
	for(i=0; i<np; i++)
	{
		double edgLng;
		edgLng=(p[i]-*prv).GetLength();
		avgLng+=edgLng;

		for(j=0; j<nEdgSortTable; j++)
		{
			if(edgLng>edgSortLng[j])
			{
				if(nEdgSortTable>=maxNumEdgSortTable)
				{
					nEdgSortTable--;
				}
				for(k=nEdgSortTable; k>j; k--)
				{
					edgSortIdx[k]=edgSortIdx[k-1];
					edgSortLng[k]=edgSortLng[k-1];
				}
				edgSortIdx[j]=edId;
				edgSortLng[j]=edgLng;
				goto NEXTPOINT_PREPROCESS;
			}
		}

		if(nEdgSortTable<maxNumEdgSortTable)
		{
			edgSortIdx[nEdgSortTable]=edId;
			edgSortLng[nEdgSortTable]=edgLng;
			nEdgSortTable++;
		}
		// printf("Edge Length[%d]=%lf\n",i,(p[(i+1)%np]-p[i]).GetLength());
	NEXTPOINT_PREPROCESS:
		prv=&(p[i]);
		edId=i;
	}
	avgLng/=double(np);
	// printf("Average Edge Length=%lf\n",avgLng);



	ang=0.0;
	o1=&pnt;
	for(i=0; i<256; i++)
	{
		if(i<nEdgSortTable)
		{
			p1=&p[edgSortIdx[i]];
			p2=&p[(edgSortIdx[i]+1)%np];
			v1=((*p1)+(*p2))/2.0-(*o1);
			if(v1.GetSquareLength()<YsSqr(YsTolerance))  // Added 2002/08/21
			{
				continue;
			}
		}
		else
		{
			v1.Set(cos(ang),sin(ang));
		}

		CrossCounter=0;
		for(j=0; j<np; j++)
		{
			vt1= j;
			vt2=(j+1)%np;

			o2=&p[vt1];
			v2=p[vt2]-p[vt1];

			if(pnt==p[vt1] || pnt==p[vt2])
			{
				return YSBOUNDARY;
			}

			if((p[vt1]-(*o1))*v1<0.0 && (p[vt2]-(*o1))*v1<0.0)  // Added 2001/04/19
			{
				continue;
			}

			edSqLen=(p[vt2]-p[vt1]).GetSquareLength();

			if(p[vt1]==p[vt2])
			{
				continue;
			}

			if(YsGetLineIntersection2(crs,*o1,(*o1)+v1,*o2,(*o2)+v2)==YSOK)
			{
				if(crs==pnt && YsCheckInBetween2(crs,p[vt2],p[vt1])==YSTRUE)
				{
					return YSBOUNDARY;
				}

				cVec=crs-pnt;

				//  2001/03/07 the following rule is changed to 1% rule
				//if(crs==p[vt1] || crs==p[vt2])
				//{
				//	goto NEXTANGLE;
				//}
				if(cVec*v1>0.0 &&
				   ((crs-p[vt1]).GetSquareLength()<edSqLen/10000.0 ||
				    (crs-p[vt2]).GetSquareLength()<edSqLen/10000.0))
				{
					goto NEXTANGLE;
				}

				if(cVec*v1>0.0 && YsCheckInBetween2(crs,p[vt1],p[vt2])==YSTRUE)
				{
					// 2001/04/19
					// If the ray hits very short edge (shorter than 1% of
					// average length), try next one.
					if(edSqLen<YsSqr(avgLng/100.0))
					{
						goto NEXTANGLE;
					}
					CrossCounter++;
				}
			}
			else if(YsCheckLineOverlap2(*o1,v1,*o2,v2)==YSTRUE)
			{
				goto NEXTANGLE;
			}
		}
		if(CrossCounter%2==0)
		{
			return YSOUTSIDE;
		}
		else
		{
			return YSINSIDE;
		}

	NEXTANGLE:
		ang+=(92.0*YsPi/180.0);
	}
	YsErrOut("YsCheckInsidePolyg2\nWarning (Probably shrunk polygon)\n");
	return YSUNKNOWNSIDE;
}


////////////////////////////////////////////////////////////
YSSIDE YsCheckInsideTriangle2(const YsVec2 &ref,const YsVec2 tri[])
{
	if(tri[0]==ref || tri[1]==ref || tri[2]==ref)
	{
		return YSBOUNDARY;
	}

	if(YsCheckInBetween2(ref,tri[0],tri[1])==YSTRUE &&
	   YsGetPointLineDistance2(tri[0],tri[1],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(YsCheckInBetween2(ref,tri[1],tri[2])==YSTRUE &&
	   YsGetPointLineDistance2(tri[1],tri[2],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(YsCheckInBetween2(ref,tri[2],tri[0])==YSTRUE &&
	   YsGetPointLineDistance2(tri[2],tri[0],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(tri[0]==tri[1] || tri[1]==tri[2] || tri[2]==tri[0])
	{
		return YSOUTSIDE;
	}

	YsVec2 v[3];
	double cp[3];
	v[0]=tri[0]-ref;
	v[1]=tri[1]-ref;
	v[2]=tri[2]-ref;

	cp[0]=v[0]^v[1];
	cp[1]=v[1]^v[2];
	cp[2]=v[2]^v[0];

	if(cp[0]*cp[1]>=0.0 && cp[1]*cp[2]>=0.0 && cp[2]*cp[0]>=0)
	// 2005/07/07
	//   The third condition cp[2]*cp[0] is necessary for detecting the case
	//   when a vertex is on an extension of one of three edges.  In that
	//   case, cp[0]*cp[1] or cp[1]*cp[2] may become zero.  Without the third
	//   condition, the vertex may falsely be identified as inside.
	{
		return YSINSIDE;
	}

	return YSOUTSIDE;
}

YSSIDE YsCheckInsideTriangle3(const YsVec3 &ref,const YsVec3 tri[])
{
	if(tri[0]==ref || tri[1]==ref || tri[2]==ref)
	{
		return YSBOUNDARY;
	}


	if(YsCheckInBetween3(ref,tri[0],tri[1])==YSTRUE &&
	   YsGetPointLineDistance3(tri[0],tri[1],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(YsCheckInBetween3(ref,tri[1],tri[2])==YSTRUE &&
	   YsGetPointLineDistance3(tri[1],tri[2],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(YsCheckInBetween3(ref,tri[2],tri[0])==YSTRUE &&
	   YsGetPointLineDistance3(tri[2],tri[0],ref)<=YsTolerance)
	{
		return YSBOUNDARY;
	}

	if(tri[0]==tri[1] || tri[1]==tri[2] || tri[2]==tri[0])
	{
		return YSOUTSIDE;
	}

	YsVec3 v[3],cp[3];
	v[0]=tri[0]-ref;
	v[1]=tri[1]-ref;
	v[2]=tri[2]-ref;

	cp[0]=v[0]^v[1];
	cp[1]=v[1]^v[2];
	cp[2]=v[2]^v[0];

	if(cp[0]*cp[1]>=0.0 && cp[1]*cp[2]>=0.0 && cp[2]*cp[0]>=0)
	// 2005/07/07
	//   The third condition cp[2]*cp[0] is necessary for detecting the case
	//   when a vertex is on an extension of one of three edges.  In that
	//   case, cp[0]*cp[1] or cp[1]*cp[2] may become zero.  Without the third
	//   condition, the vertex may falsely be identified as inside.
	{
		return YSINSIDE;
	}

	return YSOUTSIDE;
}

////////////////////////////////////////////////////////////

const double YsGetPolygonCornerAngle(YSSIZE_T nPlVt,const YsVec3 plVtPos[],int corner)
{
	if(0<=corner && corner<nPlVt)
	{
		YsVec3 v[2]=
		{
			plVtPos[(corner+nPlVt-1)%nPlVt]-plVtPos[corner],
			plVtPos[(corner+1)      %nPlVt]-plVtPos[corner]
		};

		v[0].Normalize();
		v[1].Normalize();
		const double dotProd=YsBound(v[0]*v[1],-1.0,1.0);
		return acos(dotProd);
	}
	return 0.0;
}

void YsGetPolygonMinAndMaxCornerAngle(double &minAng,double &maxAng,YSSIZE_T nPlVt,const YsVec3 plVtPos[])
{
	if(3<=nPlVt)
	{
		minAng=YsPi;
		maxAng=0.0;
		for(int i=0; i<nPlVt; i++)
		{
			const double ang=YsGetPolygonCornerAngle(nPlVt,plVtPos,i);
			minAng=YsSmaller(minAng,ang);
			maxAng=YsGreater(maxAng,ang);
		}
	}
	else
	{
		minAng=0.0;
		maxAng=0.0;
	}
}


YSBOOL YsIsLineSegIntersectionReliable2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &q1,const YsVec2 &q2)
{
	YsVec2 vp,vq;
	vp=p2-p1;
	vq=q2-q1;
	if(vp.Normalize()!=YSOK || vq.Normalize()!=YSOK)
	{
		return YSFALSE;
	}

	if(fabs(vp*vq)>0.999848) // 1 degree
	{
		YsVec2 nearPos;
		double sqLng;
		sqLng=(p1-p2).GetSquareLength();
		if((YsGetNearestPointOnLine2(nearPos,p1,p2,q1)==YSOK && (nearPos-q1).GetSquareLength()<sqLng/10000.0) ||
		   (YsGetNearestPointOnLine2(nearPos,p1,p2,q2)==YSOK && (nearPos-q2).GetSquareLength()<sqLng/10000.0))
		{
			return YSFALSE;
		}
		sqLng=(q1-q2).GetSquareLength();
		if((YsGetNearestPointOnLine2(nearPos,q1,q2,p1)==YSOK && (nearPos-p1).GetSquareLength()<sqLng/10000.0) ||
		   (YsGetNearestPointOnLine2(nearPos,q1,q2,p2)==YSOK && (nearPos-p2).GetSquareLength()<sqLng/10000.0))
		{
			return YSFALSE;
		}
	}

	return YSTRUE;
}

YSBOOL YsPointEdgeDistanceIsWithinRangeOf(const YsVec3 &pnt,int np,const YsVec3 p[],const double &ratio)
{
	int i;
	for(i=0; i<np; i++)
	{
		YsVec3 p1,p2,prj;
		p1=p[i];
		p2=p[(i+1)%np];

		YsGetNearestPointOnLine3(prj,p1,p2,pnt);

		double sqEdgeLength,sqEdgePntDistance;
		sqEdgeLength=(p1-p2).GetSquareLength();
		sqEdgePntDistance=(prj-pnt).GetSquareLength();

		if(sqEdgePntDistance<=sqEdgeLength*ratio*ratio)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}


YSRESULT YsGetCenterOfPolygon3(YsVec3 &cen,YSSIZE_T np,const YsVec3 plg[])
{
	const YsVec3 *tri[3];
	if(YsGetLargestTriangleFromPolygon3(tri,np,plg)==YSOK)
	{
		cen=(*tri[0]+*tri[1]+*tri[2])/3.0;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsGetCenterOfPolygon2(YsVec2 &cen,YSSIZE_T np,const YsVec2 plg[])
{
	const YsVec2 *tri[3];
	if(YsGetLargestTriangleFromPolygon2(tri,np,plg)==YSOK)
	{
		cen=(*tri[0]+*tri[1]+*tri[2])/3.0;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YsVec3 YsGetCenter(YSSIZE_T np,const YsVec3 p[])
{
	YsVec3 sum=YsOrigin();
	for(YSSIZE_T i=0; i<np; ++i)
	{
		sum+=p[i];
	}
	if(0<np)
	{
		sum/=(double)np;
	}
	return sum;
}

YsVec2 YsGetCenter(YSSIZE_T np,const YsVec2 p[])
{
	YsVec2 sum(0.0,0.0);
	for(YSSIZE_T i=0; i<np; ++i)
	{
		sum+=p[i];
	}
	if(0<np)
	{
		sum/=(double)np;
	}
	return sum;
}

YSRESULT YsGetPolygonProjectionMatrix(YsMatrix4x4 &mat,YSSIZE_T np,const YsVec3 p[])
{
	YsVec3 tri[3],cen,nom;
	if(YsGetLargestTriangleFromPolygon3(tri,np,p)==YSOK)
	{
		mat.Initialize();

		nom=((tri[1]-tri[0])^(tri[2]-tri[1]));
		nom.Normalize();
		cen=((tri[0]+tri[1]+tri[2])/3.0);

		double hdg,pch;
		hdg=atan2(nom.x(),-nom.z());
		pch=asin(nom.y());
		mat.RotateZY( pch);
		mat.RotateXZ(-hdg);

		mat.Translate(-cen);
		return YSOK;
	}
	return YSERR;
}

YSINTERSECTION YsGetLinePenetration2
    (const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &q1,const YsVec2 &q2)
{
	if(p1==q1 || p1==q2 || p2==q1 || p2==q2)
	{
		return YSSHAREPOINT;
	}
	else
	{
		// First, let's check bounding box
		double x1,y1,x2,y2;
		x1=YsSmaller(p1.x(),p2.x())-YsTolerance;
		y1=YsSmaller(p1.y(),p2.y())-YsTolerance;
		x2=YsGreater(p1.x(),p2.x())+YsTolerance;
		y2=YsGreater(p1.y(),p2.y())+YsTolerance;

		if((q1.x()<x1 && q2.x()<x1)||(q1.x()>x2 && q2.x()>x2)||
		   (q1.y()<y1 && q2.y()<y1)||(q1.y()>y2 && q2.y()>y2))
		{
			return YSAPART;
		}

		YsVec2 crs;
		if(YsGetLineIntersection2(crs,p1,p2,q1,q2)==YSOK)
		{
			if((YsCheckInBetween2(crs,p1,p2)==YSTRUE && (q1==crs||q2==crs))||
			   (YsCheckInBetween2(crs,q1,q2)==YSTRUE && (p1==crs||p2==crs)))
			{
				return YSTOUCH;
			}
			else if(YsCheckInBetween2(crs,p1,p2)==YSTRUE &&
			        YsCheckInBetween2(crs,q1,q2)==YSTRUE)
			{
				return YSINTERSECT;
			}
			else
			{
				return YSAPART;
			}
		}
		else if(YsCheckLineOverlap2(p1,p2-p1,q1,q2-q1)==YSTRUE)
		{
			return YSOVERLAP;
		}
		else
		{
			return YSAPART;
		}
	}
}

YSINTERSECTION YsGetLinePenetration3
    (const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2)
{
	if(p1==q1 || p1==q2 || p2==q1 || p2==q2)
	{
		return YSSHAREPOINT;
	}
	else
	{
		// First, let's check bounding box
		double x1,y1,z1,x2,y2,z2;
		x1=YsSmaller(p1.x(),p2.x())-YsTolerance;
		y1=YsSmaller(p1.y(),p2.y())-YsTolerance;
		z1=YsSmaller(p1.z(),p2.z())-YsTolerance;
		x2=YsGreater(p1.x(),p2.x())+YsTolerance;
		y2=YsGreater(p1.y(),p2.y())+YsTolerance;
		z2=YsGreater(p1.z(),p2.z())+YsTolerance;

		if((q1.x()<x1 && q2.x()<x1)||(q1.x()>x2 && q2.x()>x2)||
		   (q1.y()<y1 && q2.y()<y1)||(q1.y()>y2 && q2.y()>y2)||
		   (q1.z()<z1 && q2.z()<z1)||(q1.z()>z2 && q2.z()>z2))
		{
			return YSAPART;
		}

		YsVec3 crs1,crs2;
		if(YsGetNearestPointOfTwoLine(crs1,crs2,p1,p2,q1,q2)==YSOK)
		{
			if((YsCheckInBetween3(crs1,p1,p2)==YSTRUE&&(q1==crs1||q2==crs1))||
			   (YsCheckInBetween3(crs2,q1,q2)==YSTRUE&&(p1==crs2||p2==crs2)))
			{
				return YSTOUCH;
			}
			else if(YsCheckInBetween3(crs1,p1,p2)==YSTRUE &&
			        YsCheckInBetween3(crs2,q1,q2)==YSTRUE)
			{
				return YSINTERSECT;
			}
			else
			{
				return YSAPART;
			}
		}
		else if(YsCheckLineOverlap3(p1,p2-p1,q1,q2-q1)==YSTRUE)
		{
			return YSOVERLAP;
		}
		else
		{
			return YSAPART;
		}
	}
}

YSBOOL YsCheckSeparatability2(YSSIZE_T np,const YsVec2 p[],int idx1,int idx2)
{
	if(np<=3 || idx1==idx2 || idx1==(idx2+1)%np || idx2==(idx1+1)%np)
	{
		return YSFALSE;
	}
	else if(0<=idx1 && idx1<np && 0<=idx2 && idx2<np)
	{
		YsVec2 r1,r2,rc,s1,s2;
		r1=p[idx1];
		r2=p[idx2];

		rc=(r1+r2)/2.0;
		if(YsCheckInsidePolygon2(rc,np,p)==YSINSIDE)
		{
			int i;
			for(i=0; i<np; i++)
			{
				YSINTERSECTION is;
				is=YsGetLinePenetration2(p[i],p[(i+1)%np],r1,r2);
				if(is!=YSAPART && is!=YSSHAREPOINT)
				{
					return YSFALSE;
				}
			}
			return YSTRUE;
		}
	}
	return YSFALSE;
}


YSBOOL YsCheckSeparatability3(YSSIZE_T np,const YsVec3 p[],int idx1,int idx2)
{
	if(np<=3 || idx1==idx2 || idx1==(idx2+1)%np || idx2==(idx1+1)%np)
	{
		return YSFALSE;
	}
	else if(0<=idx1 && idx1<np && 0<=idx2 && idx2<np)
	{
		YsVec3 r1,r2,rc,s1,s2;
		r1=p[idx1];
		r2=p[idx2];

		rc=(r1+r2)/2.0;
		if(YsCheckInsidePolygon3(rc,np,p)==YSINSIDE)
		{
			int i;
			for(i=0; i<np; i++)
			{
				YSINTERSECTION is;
				is=YsGetLinePenetration3(p[i],p[(i+1)%np],r1,r2);
				if(is!=YSAPART && is!=YSSHAREPOINT)
				{
					return YSFALSE;
				}
			}
			return YSTRUE;
		}
	}
	return YSFALSE;

/* Project to XY plane method
	int i;
	YsVec3 prj;
	YsVec2 *p2d;
	YsMatrix4x4 mat;
	YSBOOL separatability;
	YsOneMemoryBlock <YsVec2,128> memBlock;


	if(idx1==idx2 || idx1==(idx2+1)%np || idx2==(idx1+1)%np)
	{
		return YSFALSE;
	}

	if(YsGetPolygonProjectionMatrix(mat,np,p)==YSOK)
	{
		p2d=memBlock.GetMemoryBlock(np);

		if(p2d!=NULL)
		{
			for(i=0; i<np; i++)
			{
				prj=mat*p[i];
				p2d[i].Set(prj.x(),prj.y());
			}

			separatability=YsCheckSeparatability2(np,p2d,idx1,idx2);

			return separatability;
		}
	}
	return YSFALSE;
*/
}


YSBOOL YsCheckBoundingBoxCollision3(
    const YsVec3 &min1,const YsVec3 &max1,const YsVec3 &min2,const YsVec3 &max2)
{
	if(max1.x()<min2.x()-YsTolerance || max2.x()<min1.x()-YsTolerance ||
	   max1.y()<min2.y()-YsTolerance || max2.y()<min1.y()-YsTolerance ||
	   max1.z()<min2.z()-YsTolerance || max2.z()<min1.z()-YsTolerance)
	{
		return YSFALSE;
	}
	return YSTRUE;
}

YSBOOL YsCheckBoundingBoxCollision2(
    const YsVec2 &min1,const YsVec2 &max1,const YsVec2 &min2,const YsVec2 &max2)
{
	if(max1.x()<min2.x()-YsTolerance || max2.x()<min1.x()-YsTolerance ||
	   max1.y()<min2.y()-YsTolerance || max2.y()<min1.y()-YsTolerance)
	{
		return YSFALSE;
	}
	return YSTRUE;
}


YSBOOL YsCheckInsideBoundingBox3(
    const YsVec3 &ref,const YsVec3 &min,const YsVec3 &max)
{
	if(min.x()-YsTolerance<=ref.x() && ref.x()<=max.x()+YsTolerance &&
	   min.y()-YsTolerance<=ref.y() && ref.y()<=max.y()+YsTolerance &&
	   min.z()-YsTolerance<=ref.z() && ref.z()<=max.z()+YsTolerance)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsCheckInsideBoundingBox2(
    const YsVec2 &ref,const YsVec2 &min,const YsVec2 &max)
{
	if(min.x()-YsTolerance<=ref.x() && ref.x()<=max.x()+YsTolerance &&
	   min.y()-YsTolerance<=ref.y() && ref.y()<=max.y()+YsTolerance)
	{
		return YSTRUE;
	}
	return YSFALSE;
}


YSFLIPDIRECTION YsCheckFlipDirection2(YSSIZE_T np,const YsVec2 p[])
{
	int i,j;
	for(i=0; i<np; i++)
	{
		const YsVec2 *vt1,*vt2;
		vt1=&p[i];
		vt2=&p[(i+1)%np];
		if(*vt1==*vt2)
		{
			continue;
		}

		for(j=1; j<4; j++)
		{
			int k,nCrs;
			YsVec2 c,t,n;
			t=(*vt2-*vt1);
			c=*vt1+t*double(j)/4.0;
			n.Set(t.y(),-t.x());

			nCrs=0;
			for(k=0; k<np; k++)
			{
				const YsVec2 *r1,*r2;
				YsVec2 is;
				r1=&p[k];
				r2=&p[(k+1)%np];

				if(k==i || *r1==*r2)
				{
					continue;
				}

				if((*r2-c)*n<0.0 && (*r1-c)*n<0.0)
				{
					continue;
				}

				if(YsGetLineIntersection2(is,c,c+n,*r1,*r2)==YSOK)
				{
					if(is==c || is==*r1 || is==*r2)
					{
						nCrs=-1;
						break;
					}
					if(YsCheckInBetween2(is,*r1,*r2)==YSTRUE &&
					   (is-c)*n>=0.0)
					{
						nCrs++;
					}
				}
				else if(YsCheckLineOverlap2(*vt1,*vt2-*vt1,*r1,*r2-*r1)==YSTRUE)
				{
					nCrs=-1;
					break;
				}
			}
			if(nCrs>=0 && nCrs%2!=0)
			{
				return YSFLIPCW;
			}
			else if(nCrs>=0 && nCrs%2==0)
			{
				return YSFLIPCCW;
			}
		}
	}
	return YSFLIPUNKNOWN;
}

YSFLIPDIRECTION YsCheckFlipDirection3(YSSIZE_T np,const YsVec3 p[],const YsVec3 &n)
{
	int i;
	double hdg,pch;
	YsVec3 nom;
	YsVec2 *q;
	YsMatrix4x4 mat;
	YSFLIPDIRECTION flip;

	YsOneMemoryBlock <YsVec2,64> memBlock;

	if(np>=3 && n.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		nom=n;
		nom.Normalize();

		hdg=atan2(nom.x(),-nom.z());
		pch=asin(nom.y());
		mat.Initialize();
		mat.RotateZY( pch);
		mat.RotateXZ(-hdg);

		q=memBlock.GetMemoryBlock(np);

		if(q!=NULL)
		{
			for(i=0; i<np; i++)
			{
				YsVec3 prj;
				prj=mat*p[i];
				q[i].Set(prj.x(),prj.y());
			}
			flip=YsCheckFlipDirection2(np,q);

			if(YsCoordSysModel==YSLEFT_ZPLUS_YPLUS)
			{
				if(flip==YSFLIPCCW)
				{
					flip=YSFLIPCW;
				}
				else if(flip==YSFLIPCW)
				{
					flip=YSFLIPCCW;
				}
			}
			else if(YsCoordSysModel==YSRIGHT_ZMINUS_YPLUS)
			{
				// Right Hand -> Don't have to be inversed.
			}
			else
			{
				YsErrOut(
					"YsCheckFlipDirection3()\n"
					"Unknown Coordinate System Model\n"
					"Assuming Right hand\n");
			}

			return flip;
		}
		else
		{
			YsErrOut("YsCheckFlipDirection3()\nLowMemoryWarning!!\n");
		}
	}
	return YSFLIPUNKNOWN;

}


YSRESULT YsGetTwoPlaneCrossLine
    (YsVec3 &org,YsVec3 &vec,const YsPlane &pl1,const YsPlane &pl2)
{
	/******************************************************************
	Line Direction Vector 'lin->v' is Outer Product of 'pl1->n','pl2->n'.

	'p1l' is the line that crosses pl1->o and whose direction vector is
	the Outer Product of 'pl1->n' and 'lin->v'. The point which 'p1l' and
	'pl2' cross will be 'lin->o'.
	******************************************************************/

	YsVec3 org1,nom1,org2,nom2;
	pl1.GetOrigin(org1);
	pl1.GetNormal(nom1);
	pl2.GetOrigin(org2);
	pl2.GetNormal(nom2);

	vec=nom1^nom2;
	if(vec.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		vec.Normalize();

		YsVec3 guide;
		guide=nom1^vec;

		if(guide.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
		{
			guide.Normalize();
			if(pl2.GetIntersection(org,org1,guide)==YSOK)
			{
				return YSOK;
			}
		}
	}
	return YSERR;
}


YSRESULT YsGetNearestPointOfTwoLine
    (YsVec3 &np,YsVec3 &nq,
     const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2)
{
	double a,b,c,d,e,f,s,t;
	YsVec3 pv,qv;

	pv=p2-p1;
	qv=q2-q1;

	a= pv*pv;
	b=-qv*pv;
	c= p1*pv-q1*pv;
	d= pv*qv;
	e=-qv*qv;
	f= p1*qv-q1*qv;

	if(YsSolveTwoLinearEquation(&t,&s,a,b,c,d,e,f)==YSOK)
	{
		np=p1+t*pv;
		nq=q1+s*qv;
		return YSOK;
	}
	else
	{
		np=p1;
		YsGetNearestPointOnLine3(nq,q1,q2,np);
	}
	return YSERR;
}

YSRESULT YsGetNearestPointOnLine3
    (YsVec3 &np,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref)
{
	double l;
	YsVec3 pv,rv;

	pv=p2-p1;
	if(pv.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		pv.Normalize();
		rv=ref-p1;
		l=rv*pv;
		np=p1+pv*l;
		return YSOK;
	}
	np=p1;
	return YSERR;
}

YSRESULT YsGetNearestPointOnLine2
    (YsVec2 &np,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &ref)
{
	double l;
	YsVec2 pv,rv;

	pv=p2-p1;
	if(pv.GetSquareLength()>YsSqr(YsTolerance))  // Modified 2000/11/10
	{
		pv.Normalize();
		rv=ref-p1;
		l=rv*pv;
		np=p1+pv*l;
		return YSOK;
	}
	np=p1;
	return YSERR;
}

const double YsGetPointLineDistance2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &ref)
{
	YsVec2 v,prj;
	v=p2-p1;
	if(v.Normalize()==YSOK)
	{
		prj=p1+v*((ref-p1)*v);
		return (prj-ref).GetLength();
	}
	return 0.0;
}

const double YsGetPointLineDistance3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref)
{
	YsVec3 v,prj;
	v=p2-p1;
	if(v.Normalize()==YSOK)
	{
		prj=p1+v*((ref-p1)*v);
		return (prj-ref).GetLength();
	}
	return 0.0;
}

double YsGetDistanceBetweenTwoLineSegment(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2)
{
	double dist;
	YsVec3 np,nq;

	if(p1==q1 || p1==q2 || p2==q1 || p2==q2)
	{
		return 0.0;
	}

	if(YsGetNearestPointOfTwoLine(np,nq,p1,p2,q1,q2)==YSOK)
	{
		if(YsCheckInBetween3(np,p1,p2)==YSTRUE && YsCheckInBetween3(nq,q1,q2)==YSTRUE)
		{
			return (np-nq).GetLength();
		}
	}

	dist=               (p1-q1).GetLength();
	dist=YsSmaller(dist,(p1-q2).GetLength());
	dist=YsSmaller(dist,(p2-q1).GetLength());
	dist=YsSmaller(dist,(p2-q2).GetLength());

	if(YsGetNearestPointOnLine3(np,p1,p2,q1)==YSOK && YsCheckInBetween3(np,p1,p2)==YSTRUE)
	{
		dist=YsSmaller(dist,(np-q1).GetLength());
	}
	if(YsGetNearestPointOnLine3(np,p1,p2,q2)==YSOK && YsCheckInBetween3(np,p1,p2)==YSTRUE)
	{
		dist=YsSmaller(dist,(np-q2).GetLength());  // Bug Fix:  q1 -> q2   Credit to Ved.
	}
	if(YsGetNearestPointOnLine3(nq,q1,q2,p1)==YSOK && YsCheckInBetween3(nq,q1,q2)==YSTRUE)
	{
		dist=YsSmaller(dist,(nq-p1).GetLength());
	}
	if(YsGetNearestPointOnLine3(nq,q1,q2,p2)==YSOK && YsCheckInBetween3(nq,q1,q2)==YSTRUE)
	{
		dist=YsSmaller(dist,(nq-p2).GetLength());
	}

	return dist;
}

const double YsGetPointLineSegDistance2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &ref)
{
	if(YsCheckInBetween2(ref,p1,p2)==YSTRUE)
	{
		YsVec2 n;
		if(YsGetNearestPointOnLine2(n,p1,p2,ref)==YSOK)
		{
			return (n-ref).GetLength();
		}
	}

	double d1,d2;
	d1=(ref-p1).GetSquareLength();
	d2=(ref-p2).GetSquareLength();
	if(d1<d2)
	{
		return sqrt(d1);
	}
	else
	{
		return sqrt(d2);
	}
}

const double YsGetPointLineSegDistance3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref)
{
	if(YsCheckInBetween3(ref,p1,p2)==YSTRUE)
	{
		YsVec3 n;
		if(YsGetNearestPointOnLine3(n,p1,p2,ref)==YSOK)
		{
			return (n-ref).GetLength();
		}
	}

	double d1,d2;
	d1=(ref-p1).GetSquareLength();
	d2=(ref-p2).GetSquareLength();
	if(d1<d2)
	{
		return sqrt(d1);
	}
	else
	{
		return sqrt(d2);
	}
}

const double YsGetPointLineSegDistance3(const YsVec3 p[2],const YsVec3 &ref)
{
	return YsGetPointLineSegDistance3(p[0],p[1],ref);
}

YsVec3 YsGetNearestPointOnLinePiece(const YsVec3 edVtPos[2],const YsVec3 &from)
{
	YsVec3 nearPos;
	if(YSOK==YsGetNearestPointOnLine3(nearPos,edVtPos[0],edVtPos[1],from) &&
	   YSTRUE==YsCheckInBetween3(nearPos,edVtPos))
	{
		return nearPos;
	}
	else
	{
		if((edVtPos[0]-from).GetSquareLength()<(edVtPos[1]-from).GetSquareLength())
		{
			return edVtPos[0];
		}
		else
		{
			return edVtPos[1];
		}
	}
}

YsVec2 YsGetNearestPointOnLinePiece(const YsVec2 edVtPos[2],const YsVec2 &from)
{
	YsVec2 nearPos;
	if(YSOK==YsGetNearestPointOnLine2(nearPos,edVtPos[0],edVtPos[1],from) &&
	   YSTRUE==YsCheckInBetween2(nearPos,edVtPos))
	{
		return nearPos;
	}
	else
	{
		if((edVtPos[0]-from).GetSquareLength()<(edVtPos[1]-from).GetSquareLength())
		{
			return edVtPos[0];
		}
		else
		{
			return edVtPos[1];
		}
	}
}

YSBOOL YsCheckPointIsOnLineSegment3(const YsVec3 &p,const YsVec3 &p1,const YsVec3 &p2)
{
	if(p1==p || p2==p)
	{
		return YSTRUE;
	}

	if(YsCheckInBetween3(p,p1,p2)==YSTRUE)
	{
		YsVec3 n;
		if(YsGetNearestPointOnLine3(n,p1,p2,p)==YSOK && n==p)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

static YSINTERSECTION YsGetPolygonEdgeRelation
(YSSIZE_T np,const YsVec3 p[],const YsPlane &pln,const YsVec3 &e1,const YsVec3 &e2)
{
	if(e1!=e2 &&
	   pln.CheckOnPlane(e1)==YSTRUE &&
	   pln.CheckOnPlane(e2)==YSTRUE)
	{
		YSSIDE side1,side2;
		side1=YsCheckInsidePolygon3(e1,np,p);
		side2=YsCheckInsidePolygon3(e2,np,p);
		if(side1==YSINSIDE || side2==YSINSIDE)
		{
			return YSTOUCH;  // 2000/03/13  YSINTERSECT;  Cannot say intersect at this point
		}
		else if(side1==YSBOUNDARY || side2==YSBOUNDARY)
		{
			return YSTOUCH;
		}
		else
		{
			if(side1==YSUNKNOWNSIDE || side2==YSUNKNOWNSIDE)
			{
				YsErrOut("YsGetPolygonEdgeRelation\n"
				                      "  Seems bad polygon(1)\n");
			}

			// Even if it misses the case like
			//     e1    |(polygon boundary)|     e2
			// Another polygon will hit this case.
			return YSAPART;
		}
	}

	// OK, the edge is not parallel to the polygon.

	if(pln.CheckOnPlane(e1)==YSTRUE)
	{
		YSSIDE side1;
		side1=YsCheckInsidePolygon3(e1,np,p);
		if(side1==YSINSIDE || side1==YSBOUNDARY)
		{
			return YSTOUCH;
		}
		if(side1==YSUNKNOWNSIDE)
		{
			YsErrOut("YsGetPolygonEdgeRelation\n  Seems bad polygon(2)\n");
		}
	}
	if(pln.CheckOnPlane(e2)==YSTRUE)
	{
		YSSIDE side2;
		side2=YsCheckInsidePolygon3(e2,np,p);
		if(side2==YSINSIDE || side2==YSBOUNDARY)
		{
			return YSTOUCH;
		}
		if(side2==YSUNKNOWNSIDE)
		{
			YsErrOut("YsGetPolygonEdgeRelation\n  Seems bad polygon(3)\n");
		}
	}

	// OK, at this point, both end points are not touching the plane.

	if(e1==e2)
	{
		return YSAPART;
	}

	// Not parallel, end points are not touching, the edge has length.

	YsVec3 crs;
	if(pln.GetPenetration(crs,e1,e2)==YSOK)
	{
		YSSIDE side;
		side=YsCheckInsidePolygon3(crs,np,p);
		if(side==YSINSIDE)
		{
			return YSINTERSECT;
		}
		else if(side==YSBOUNDARY)
		{
			return YSTOUCH;
		}
		else if(side==YSUNKNOWNSIDE)
		{
			YsErrOut("YsGetPolygonEdgeRelation\n  Seems bad polygon(4)\n");
		}
	}

	return YSAPART;
}


static YSINTERSECTION YsGetPolygonPenetration_CheckObviousIntersection1to2
    (int &nTouch,YSSIZE_T np1,const YsVec3 p1[],const YsPlane &pln1,YSSIZE_T np2,const YsVec3 p2[])
{
	YSINTERSECTION isect;
	int i;
	nTouch=0;
	for(i=0; i<np2; i++)
	{
		isect=YsGetPolygonEdgeRelation(np1,p1,pln1,p2[i],p2[(i+1)%np2]);
		if(isect==YSINTERSECT)
		{
			return YSINTERSECT;
		}
		else if(isect==YSTOUCH)
		{
			nTouch++;
		}
	}
	return YSAPART; // It's not the final conclusion.
}

static YSBOOL YsGetPolygonPenetration_CheckAllVtxOfPolyg1IsOneSideOfPolyg2
    (YSBOOL &side1,YSBOOL &side2,YSSIZE_T np1,const YsVec3 p1[],const YsPlane &pln2)
{
	int i;

	side1=YSFALSE;
	side2=YSFALSE;

	for(i=0; i<np1; i++)
	{
		if(pln2.CheckOnPlane(p1[i])!=YSTRUE)
		{
			if(((p1[i]-pln2.GetOrigin())*pln2.GetNormal())>0.0)
			{
				side1=YSTRUE;
			}
			else
			{
				side2=YSTRUE;
			}
		}
	}
	if(side1!=side2)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

static YSBOOL YsGetPolygonPenetration_Overlapping(YSSIZE_T np1,const YsVec3 p1[],YSSIZE_T np2,const YsVec3 p2[])
{
	int i,j;
	YsVec3 a,b,c,d,n1,n2;
	for(i=0; i<np1; i++)
	{
		a=p1[i];
		b=p1[(i+1)%np1];
		for(j=0; j<np2; j++)
		{
			c=p2[j];
			d=p2[(j+1)%np2];

			if(YsGetNearestPointOfTwoLine(n1,n2,a,b,c,d)==YSOK &&
			   YsCheckInBetween3(n1,a,b)==YSTRUE &&
			   YsCheckInBetween3(n2,c,d)==YSTRUE)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

static YSBOOL YsGetPolygonPenetration_ExhaustiveCheckIntersectionPolyg1To2
    (YSSIZE_T np1,const YsVec3 p1[],YSSIZE_T np2,const YsVec3 p2[],const YsPlane &pln2)
{
	YSSIZE_T i;
	YsArray <YsVec3> isect;

	for(i=0; i<np1; i++)
	{
		if(pln2.CheckOnPlane(p1[i])==YSTRUE)
		{
			isect.AppendItem(p1[i]);
		}
		else if(pln2.CheckOnPlane(p1[(i+1)%np1])!=YSTRUE)
		{
			YsVec3 x;
			if(pln2.GetPenetration(x,p1[i],p1[(i+1)%np1])==YSOK)
			{
				isect.AppendItem(x);
			}
		}
	}

	YsSortPointSet3(isect.GetNumItem(),isect.GetEditableArray(),NULL,isect.GetItem(0));

	for(i=0; i<isect.GetNumItem()-1; i++)
	{
		YsVec3 c;
		c=(isect.GetItem(i)+isect.GetItem(i+1))/2.0;
		if(YsCheckInsidePolygon3(c,np1,p1)==YSINSIDE &&  // Don't take YSBOUNDARY here. I already know they are touching.
		   YsCheckInsidePolygon3(c,np2,p2)==YSINSIDE)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

/* Use all edges and separatable combinations to check penetration */
YSINTERSECTION YsGetPolygonPenetration
    (YSSIZE_T np1,const YsVec3 p1[],YSSIZE_T np2,const YsVec3 p2[])
{
	YsPlane pln1,pln2;
	int touchCount1=0,touchCount2=0;
	YSRESULT plnErr1,plnErr2;

	plnErr1=pln1.MakeBestFitPlane(np1,p1);
	plnErr2=pln2.MakeBestFitPlane(np2,p2);

	if((plnErr1==YSOK && YsGetPolygonPenetration_CheckObviousIntersection1to2(touchCount1,np1,p1,pln1,np2,p2)==YSINTERSECT) ||
       (plnErr2==YSOK && YsGetPolygonPenetration_CheckObviousIntersection1to2(touchCount2,np2,p2,pln2,np1,p1)==YSINTERSECT))
	{
//printf("Case1\n");
		return YSINTERSECT;
	}

	if(touchCount1==0 && touchCount2==0)
	{
//printf("Case2\n");
		return YSAPART;
	}


	// At this point, possibilities are YSTOUCH or YSINTERSECT. Not YSAPART
	// If, all vtx of plgA{plgB} are in the one side of plgB{plgA}, they cannot intersect each other. I.E. touching.
	YSBOOL side1=YSTFUNKNOWN,side2=YSTFUNKNOWN;
	YSBOOL overlap;
	overlap=YSFALSE;
	if(plnErr2==YSOK && YsGetPolygonPenetration_CheckAllVtxOfPolyg1IsOneSideOfPolyg2(side1,side2,np1,p1,pln2)==YSTRUE)
	{
//printf("Case3\n");
		return YSTOUCH;
	}
	else if(side1!=YSTRUE && side2!=YSTRUE)
	{
		overlap=YSTRUE;
	}

	if(plnErr1==YSOK && YsGetPolygonPenetration_CheckAllVtxOfPolyg1IsOneSideOfPolyg2(side1,side2,np2,p2,pln1)==YSTRUE)
	{
		return YSTOUCH;
	}
	else if(side1!=YSTRUE && side2!=YSTRUE)
	{
		overlap=YSTRUE;
	}

	// Overlapping?
	if(overlap==YSTRUE)
	{
		if(YsGetPolygonPenetration_Overlapping(np1,p1,np2,p2)==YSTRUE)
		{
//printf("Case4\n");
			return YSTOUCH;  // Not penetrate but overlap==touching
		}
		else
		{
//printf("Case5\n");
			return YSAPART;
		}
	}


	// At this point, I know....
	//    1. They are not apart from each other
	//    2. They are not parallel to each other
	//    3. They are touching or intersecting each other

	if((plnErr2==YSOK && YsGetPolygonPenetration_ExhaustiveCheckIntersectionPolyg1To2(np1,p1,np2,p2,pln2)==YSTRUE) ||
	   (plnErr1==YSOK && YsGetPolygonPenetration_ExhaustiveCheckIntersectionPolyg1To2(np2,p2,np1,p1,pln1)==YSTRUE))
	{
//printf("Case6\n");
		return YSINTERSECT;
	}
	else
	{
//printf("Case7\n");
		return YSTOUCH;
	}
}

////////////////////////////////////////////////////////////

YSRESULT YsGetArbitraryInsidePointOfPolygon2
     (YsVec2 &aip,YSSIZE_T np,const YsVec2 p[],YSBOOL strictConvexityCheck)
{
	if(YsCheckConvex2(np,p,strictConvexityCheck)==YSTRUE)
	{
		int i;
		YsVec2 cg;
		cg.Set(0.0,0.0);
		for(i=0; i<np; i++)
		{
			cg=cg+p[i];
		}
		cg=cg/double(np);
		aip=cg;
		return YSOK;
	}
	else
	{
		int i,j;
		for(i=0; i<np; i++)
		{
			for(j=i+1; j<np; j++)
			{
				if(YsCheckSeparatability2(np,p,i,j)==YSTRUE)
				{
					aip=(p[i]+p[j])/2.0;
					goto DONE;
				}
			}
		}


		// Separatability Check didn't work 2010/04/13 >>
		for(i=0; i<np; i++)
		{
			YsVec2 midEdge,edgeVec;
			midEdge=(p[i]+p[(i+1)%np])/2.0;
			edgeVec=p[(i+1)%np]-p[i];
			if(edgeVec.Normalize()==YSOK)
			{
				const YsVec2 rayVec(-edgeVec.y(),edgeVec.x());

				// Shoot a ray from midEdge to rayVec and -rayVec
				YSBOOL itsc[2]={YSFALSE,YSFALSE};
				YsVec2 firstItsc[2];
				double itscDist[2];
				for(j=0; j<np; j++)
				{
					if(i!=j)
					{
						YsVec2 tst;
						if(YSOK==YsGetLineIntersection2(tst,p[j],p[(j+1)%np],midEdge,midEdge+rayVec) &&
						   YSTRUE==YsCheckInBetween2(tst,p[j],p[(j+1)%np]))
						{
							if(tst==p[j] || tst==p[(j+1)%np])
							{
								itsc[0]=YSFALSE;
								itsc[1]=YSFALSE;
								break;
							}

							int side=-1;
							if(YsTolerance<(tst-midEdge)*rayVec)
							{
								side=0;
							}
							else if(-YsTolerance>(tst-midEdge)*rayVec)
							{
								side=1;
							}
							if(0<=side)
							{
								const double d=fabs(fabs((tst-midEdge)*rayVec));
								if(YSTRUE!=itsc[side])
								{
									itsc[side]=YSTRUE;
									firstItsc[side]=tst;
									itscDist[side]=d;
								}
								else if(d<itscDist[side])
								{
									firstItsc[side]=tst;
									itscDist[side]=d;
								}

								if(YSTRUE==itsc[0] && YSTRUE==itsc[1])
								{
									break;
								}
							}
						}
					}
				}

				int in=-1;
				if(YSTRUE==itsc[0] && YSTRUE!=itsc[1])
				{
					in=0;
				}
				else if(YSTRUE==itsc[1] && YSTRUE!=itsc[0])
				{
					in=1;
				}
				if(0<=in)
				{
					aip=(firstItsc[in]+midEdge)/2.0;
					if(aip!=firstItsc[in] && aip!=midEdge)
					{
						goto DONE;
					}
				}
			}
		}
		// Separatability Check didn't work 2010/04/13 <<

		YsGetCenterOfPolygon2(aip,np,p);  // I can do nothing at this point(T_T)
		return YSERR;
DONE:
		return YSOK;
	}
}

YSRESULT YsGetArbitraryInsidePointOfPolygon3
  (YsVec3 &aip,YSSIZE_T np,const YsVec3 p[],YSBOOL strictConvexityCheck)
{
	if(YsCheckConvex3(np,p,strictConvexityCheck)==YSTRUE)
	{
		int i;
		YsVec3 cg;
		cg.Set(0.0,0.0,0.0);
		for(i=0; i<np; i++)
		{
			cg=cg+p[i];
		}
		cg=cg/double(np);
		aip=cg;
		return YSOK;
	}
	else
	{
		int i,j;
		for(i=0; i<np; i++)
		{
			for(j=i+1; j<np; j++)
			{
				if(YsCheckSeparatability3(np,p,i,j)==YSTRUE)
				{
					aip=(p[i]+p[j])/2.0;
					goto DONE;
				}
			}
		}
		YsGetCenterOfPolygon3(aip,np,p);  // I can do nothing at this point(T_T)
		return YSERR;
DONE:
		return YSOK;
	}
}

////////////////////////////////////////////////////////////

YSRESULT YsSortPointSet3(YSSIZE_T np,YsVec3 p[],int idx[])
{
	return YsSortPointSet3Template <int> (np,p,idx);
}

YSRESULT YsSortPointSet3(YSSIZE_T np,YsVec3 p[],int idx[],const YsVec3 &refp)
{
	return YsSortPointSet3Template <int> (np,p,idx,refp);
}

YSRESULT YsSortPointSet3(YsVec3 p[],int idx[],YSSIZE_T np,const YsVec3 refp,double sqKnownMaxDistance)
{
	return YsSortPointSet3Template <int> (p,idx,np,refp,sqKnownMaxDistance);
}


////////////////////////////////////////////////////////////


YSRESULT YsGetCircumCircle(YsVec2 &cen,double &r,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3)
{
	double a1,b1,d1;
	double a2,b2,d2;
	double a3,b3,d3;

	a1=2.0*p1.x();
	b1=2.0*p1.y();
	d1=p1.x()*p1.x()+p1.y()*p1.y();

	a2=2.0*p2.x();
	b2=2.0*p2.y();
	d2=p2.x()*p2.x()+p2.y()*p2.y();

	a3=2.0*p3.x();
	b3=2.0*p3.y();
	d3=p3.x()*p3.x()+p3.y()*p3.y();

	// a1 xc + b1 yc + w = d1
	// a2 xc + b2 yc + w = d2
	// a3 xc + b3 yc + w = d3
	// Find xc,yc -> r=sqrt(w+xc*xc+yc*yc)


	YsMatrixTemplate <3,3> coe;
	YsMatrixTemplate <3,1> sol,rhs;
	// static YsMatrix coe,sol,rhs;
	// coe.CreateWithoutClear(3,3);
	// sol.CreateWithoutClear(3,1);
	// rhs.CreateWithoutClear(3,1);

	coe.Set(1,1,a1);
	coe.Set(1,2,b1);
	coe.Set(1,3,1.0);

	coe.Set(2,1,a2);
	coe.Set(2,2,b2);
	coe.Set(2,3,1.0);

	coe.Set(3,1,a3);
	coe.Set(3,2,b3);
	coe.Set(3,3,1.0);

	rhs.Set(1,1,d1);
	rhs.Set(2,1,d2);
	rhs.Set(3,1,d3);

	if(coe.Invert()==YSOK)
	{
		sol=coe*rhs;
		cen.Set(sol.v(1,1),sol.v(2,1));
		r=sol.v(3,1)+cen.x()*cen.x()+cen.y()*cen.y();
		if(r>=0.0)  // 2005/06/23 For preventing lock up in Linux
		{
			r=sqrt(r);
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT YsGetInscribedCircle(YsVec2 &cen,double &rad,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3)
{
	// Radius of the inscribed circle is given by a projection (C-Pn)*Nm {Nm:Normal of face/edge}
	// Equate three of them to get 3x3 matrix

	YsVec2 n1,n2,n3;

	n1=p3-p2;
	n2=p1-p3;
	n3=p2-p1;

	n1.Set(-n1.y(), n1.x());
	n1.Normalize();
	n2.Set(-n2.y(), n2.x());
	n2.Normalize();
	n3.Set(-n3.y(), n3.x());
	n3.Normalize();


	YsMatrixTemplate <3,3> coe;
	YsMatrixTemplate <3,1> sol,rhs;
	//  static YsMatrix coe,sol,rhs;
	//  coe.CreateWithoutClear(3,3);
	//  sol.CreateWithoutClear(3,1);
	//  rhs.CreateWithoutClear(3,1);

	coe.Set(1,1,n3.x());
	coe.Set(1,2,n3.y());
	coe.Set(1,3,-1.0);
	rhs.Set(1,1,p1*n3);

	coe.Set(2,1,n2.x());
	coe.Set(2,2,n2.y());
	coe.Set(2,3,-1.0);
	rhs.Set(2,1,p1*n2);

	coe.Set(3,1,n1.x());
	coe.Set(3,2,n1.y());
	coe.Set(3,3,-1.0);
	rhs.Set(3,1,p2*n1);

	if(coe.Invert()==YSOK)
	{
		sol=coe*rhs;
		cen.Set(sol.v(1,1),sol.v(2,1));
		rad=YsAbs(sol.v(3,1));   // Possibly get negative depend on flip direction of the triangle
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGetCircumSphere(YsVec3 &cen,double &rad,YsVec3 const *p[4])
{
	// 2012/05/14 Took equation from wikipedia (Search for "tetrahedron")

	YsVec3 a=(*p[1]-*p[0]);
	YsVec3 b=(*p[2]-*p[0]);
	YsVec3 c=(*p[3]-*p[0]);
	YsVec3 bxc=b^c;

	const double d=2*a*(bxc);
	if(0.0!=d)
	{
		cen=(*p[0])+(a*a*(bxc)+b*b*(c^a)+c*c*(a^b))/d;
		rad=(*p[0]-cen).GetLength();
		return YSOK;
	}

/*	int i;

	YsMatrixTemplate <4,4> mat;
	YsMatrixTemplate <4,1> val;
	YsMatrixTemplate <4,1> sol;
	//  static YsMatrix mat,val,sol;
	//  mat.CreateWithoutClear(4,4);
	//  val.CreateWithoutClear(4,1);
	//  sol.CreateWithoutClear(4,1);

	for(i=0; i<4; i++)
	{
		mat.Set(i+1,1,2.0*p[i]->x());
		mat.Set(i+1,2,2.0*p[i]->y());
		mat.Set(i+1,3,2.0*p[i]->z());
		mat.Set(i+1,4,1.0);

		val.Set(i+1,1,(p[i]->x()*p[i]->x())+(p[i]->y()*p[i]->y())+(p[i]->z()*p[i]->z()));
	}

	if(mat.Invert()==YSOK)
	{
		double w,d;

		sol=mat*val;
		cen.Set(sol.v(1,1),sol.v(2,1),sol.v(3,1));
		w=sol.v(4,1);

		d=(cen.x()*cen.x())+(cen.y()*cen.y())+(cen.z()*cen.z())+w;
		if(d>=0.0)   // 2005/06/23 For preventing lock up in Linux
		{
			rad=sqrt(d);
			return YSOK;
		}
	}
	else
	{
		return YSERR;
	} */

	return YSERR;
}

YSRESULT YsGetCircumSphereRadius(double &rad,YsVec3 const *p[4])
{
	// 2012/05/14 Took equation from wikipedia (Search for "tetrahedron")

	YsVec3 a=(*p[1]-*p[0]);
	YsVec3 b=(*p[2]-*p[0]);
	YsVec3 c=(*p[3]-*p[0]);

	const double d=2*a*(b^c);
	if(0!=d)
	{
		const YsVec3 numer=(a*a*(b^c)+b*b*(c^a)+c*c*(a^b));
		rad=numer.GetLength()/d;
		return YSOK;
	}

/*	// See research note 2009/07/07
	int i;
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> val;
	YsMatrixTemplate <3,1> sol;

	YsVec3 q[3];
	q[0]=(*p[0]-*p[3]);
	q[1]=(*p[1]-*p[3]);
	q[2]=(*p[2]-*p[3]);

	for(i=0; i<3; i++)
	{
		mat.Set(i+1,1,2.0*q[i].x());
		mat.Set(i+1,2,2.0*q[i].y());
		mat.Set(i+1,3,2.0*q[i].z());

		val.Set(i+1,1,q[i].GetSquareLength());
	}

	if(mat.Invert()==YSOK)
	{
		sol=mat*val;
		rad=(sol.v(1,1)*sol.v(1,1)+sol.v(2,1)*sol.v(2,1)+sol.v(3,1)*sol.v(3,1));
		if(rad>=0.0)
		{
			rad=sqrt(rad);
			return YSOK;
		}
	} */
	return YSERR;

}

YSRESULT YsGetCircumSphere(YsVec3 &cen,double &rad,const YsVec3 p[4])
{
	const YsVec3 *pp[4];
	pp[0]=&p[0];
	pp[1]=&p[1];
	pp[2]=&p[2];
	pp[3]=&p[3];
	return YsGetCircumSphere(cen,rad,pp);
}

YSRESULT YsGetCircumSphereRadius(double &rad,const YsVec3 p[4])
{
	// See research note 2009/07/07
	int i;
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> val;
	YsMatrixTemplate <3,1> sol;

	YsVec3 q[3];
	q[0]=(p[0]-p[3]);
	q[1]=(p[1]-p[3]);
	q[2]=(p[2]-p[3]);

	for(i=0; i<3; i++)
	{
		mat.Set(i+1,1,2.0*q[i].x());
		mat.Set(i+1,2,2.0*q[i].y());
		mat.Set(i+1,3,2.0*q[i].z());

		val.Set(i+1,1,q[i].GetSquareLength());
	}

	if(mat.Invert()==YSOK)
	{
		sol=mat*val;
		rad=(sol.v(1,1)*sol.v(1,1)+sol.v(2,1)*sol.v(2,1)+sol.v(3,1)*sol.v(3,1));
		if(rad>=0.0)
		{
			rad=sqrt(rad);
			return YSOK;
		}
	}
	return YSERR;

}

YSRESULT YsComputeNormalOfTriangle(YsVec3 &nom,const YsVec3 &t1,const YsVec3 &t2,const YsVec3 &t3)
{
	YsVec3 edVec1,edVec2;

	edVec1=t2-t1;
	edVec2=t3-t2;
	if(edVec1.Normalize()==YSOK && edVec2.Normalize()==YSOK)
	{
		nom=edVec1^edVec2;
		if(nom.Normalize()==YSOK)
		{
			return YSOK;
		}
	}

	edVec1=t3-t2;
	edVec2=t1-t3;
	if(edVec1.Normalize()==YSOK && edVec2.Normalize()==YSOK)
	{
		nom=edVec1^edVec2;
		if(nom.Normalize()==YSOK)
		{
			return YSOK;
		}
	}

	edVec1=t1-t3;
	edVec2=t2-t1;
	if(edVec1.Normalize()==YSOK && edVec2.Normalize()==YSOK)
	{
		nom=edVec1^edVec2;
		if(nom.Normalize()==YSOK)
		{
			return YSOK;
		}
	}

	return YSERR;
}

static YSRESULT YsComputeNormalOfTriangleOfTet
    (YsVec3 &nom,const YsVec3 &t1,const YsVec3 &t2,const YsVec3 &t3,const YsVec3 &gravCenterOfTet)
{
	YsVec3 gravCenterOfTri,vGcTetGcTri;

	gravCenterOfTri=(t1+t2+t3)/3.0;
	if(YsComputeNormalOfTriangle(nom,t1,t2,t3)==YSOK)
	{
		vGcTetGcTri=gravCenterOfTri-gravCenterOfTet;
		if(vGcTetGcTri.Normalize()==YSOK)
		{
			double ip;
			ip=vGcTetGcTri*nom;
			if(ip>YsTolerance)
			{
				return YSOK;
			}
			else if(ip<-YsTolerance)
			{
				nom=-nom;
				return YSOK;
			}
			else
			{
				return YSERR;
			}
		}
		else
		{
			return YSERR;
		}
	}
	return YSERR;

}

YSRESULT YsGetNormalOfTet(YsVec3 nom[4],YsVec3 const *p[4])
{
	YsVec3 gravCenter,edVec1,edVec2;

	gravCenter=(*p[0]+*p[1]+*p[2]+*p[3])/4.0;

	if(YsComputeNormalOfTriangleOfTet(nom[0],*p[1],*p[2],*p[3],gravCenter)==YSOK &&
	   YsComputeNormalOfTriangleOfTet(nom[1],*p[2],*p[3],*p[0],gravCenter)==YSOK &&
	   YsComputeNormalOfTriangleOfTet(nom[2],*p[3],*p[0],*p[1],gravCenter)==YSOK &&
	   YsComputeNormalOfTriangleOfTet(nom[3],*p[0],*p[1],*p[2],gravCenter)==YSOK)
	{
		return YSOK;
	}
	nom[0].Set(0.0,0.0,0.0);
	nom[1].Set(0.0,0.0,0.0);
	nom[2].Set(0.0,0.0,0.0);
	nom[3].Set(0.0,0.0,0.0);
	return YSERR;
}

YSRESULT YsGetNormalOfTet(YsVec3 nom[4],const YsVec3 p[4])
{
	YsVec3 const *pp[4];
	pp[0]=&p[0];
	pp[1]=&p[1];
	pp[2]=&p[2];
	pp[3]=&p[3];
	return YsGetNormalOfTet(nom,pp);
}

YSRESULT YsGetInscribedSphereRadius(double &rad,YsVec3 const *p[4])
{
	// 2012/05/14 Took equation from wikipedia (Search for "tetrahedron")

	YsVec3 a=(*p[1]-*p[0]);
	YsVec3 b=(*p[2]-*p[0]);
	YsVec3 c=(*p[3]-*p[0]);

	const YsVec3 bxc=b^c;
	const YsVec3 cxa=c^a;
	const YsVec3 axb=a^b;
	const double lbxc=bxc.GetLength();
	const double lcxa=cxa.GetLength();
	const double laxb=axb.GetLength();

	double inD=lbxc+lcxa+laxb+(bxc+cxa+axb).GetLength();
	if(0.0!=inD)
	{
		rad=fabs(a*bxc)/inD;
		return YSOK;
	}

	/*
	// See research note on 2009/07/07

	YsVec3 n1,n2,n3,n4,q0,q1,q2;

	q0=*p[0]-*p[3];
	q1=*p[1]-*p[3];
	q2=*p[2]-*p[3];

	n1=q1^q0;
	n2=q0^q2;
	n3=q2^q1;
	n4=(q1-q0)^(q2-q0);

	if(n1.Normalize()==YSOK &&
	   n2.Normalize()==YSOK &&
	   n3.Normalize()==YSOK &&
	   n4.Normalize()==YSOK)
	{
		YsMatrixTemplate <3,3> mat;
		YsMatrixTemplate <3,1> vec,sol;

		mat.Set(1,1,n1.x()-n2.x());
		mat.Set(1,2,n1.y()-n2.y());
		mat.Set(1,3,n1.z()-n2.z());

		mat.Set(2,1,n2.x()-n3.x());
		mat.Set(2,2,n2.y()-n3.y());
		mat.Set(2,3,n2.z()-n3.z());

		mat.Set(3,1,n3.x()-n4.x());
		mat.Set(3,2,n3.y()-n4.y());
		mat.Set(3,3,n3.z()-n4.z());

		vec.Set(1,1,0.0);
		vec.Set(2,1,0.0);
		vec.Set(3,1,-q0*n4);

		if(mat.Invert()==YSOK)
		{
			sol=mat*vec;
			rad=n1.x()*sol.v(1,1)+n1.y()*sol.v(2,1)+n1.z()*sol.v(3,1);
			if(rad<0.0)
			{
				rad=-rad;
			}
			return YSOK;
		}
	} */
	return YSERR;
}

YSRESULT YsGetInscribedSphere(YsVec3 &cen,double &rad,YsVec3 const *p[4])
{
	// 2012/05/14 Took equation from wikipedia (Search for "tetrahedron")

	YsVec3 a=(*p[1]-*p[0]);
	YsVec3 b=(*p[2]-*p[0]);
	YsVec3 c=(*p[3]-*p[0]);

	const YsVec3 bxc=b^c;
	const YsVec3 cxa=c^a;
	const YsVec3 axb=a^b;
	const double lbxc=bxc.GetLength();
	const double lcxa=cxa.GetLength();
	const double laxb=axb.GetLength();

	double inD=lbxc+lcxa+laxb+(bxc+cxa+axb).GetLength();
	if(0.0!=inD)
	{
		rad=fabs(a*bxc)/inD;
		cen=(*p[0])+(lbxc*a+lcxa*b+laxb*c)/inD;
		return YSOK;
	}

	/* 
	// Matrix M
	//  [ n0x   n0y   n0z   1] [cx]    [n0x*p1x+n0y*p1y+n0z*p1z]
	//  [ n2x   n2y   n2z   1] [cy]    [n2x*p1x+n2y*p1y+n2z*p1z]
	//  [ n3x   n3y   n3z   1] [cz]  = [n3x*p1x+n3y*p1y+n3z*p1z]
	//  [ n1x   n1y   n1z   1] [r ]    [n1x*p0x+n1y*p0y+n1z*p0z]

	YsVec3 n[4];

	if(YsGetNormalOfTet(n,p)==YSOK)
	{
		YsMatrixTemplate <4,4> mat;
		YsMatrixTemplate <4,1> vec;

		//  static YsMatrix mat,vec;
		//  mat.CreateWithoutClear(4,4);
		//  vec.CreateWithoutClear(4,1);

		mat.Set(1,1,n[0].x());
		mat.Set(1,2,n[0].y());
		mat.Set(1,3,n[0].z());
		mat.Set(1,4,1.0);

		mat.Set(2,1,n[2].x());
		mat.Set(2,2,n[2].y());
		mat.Set(2,3,n[2].z());
		mat.Set(2,4,1.0);

		mat.Set(3,1,n[3].x());
		mat.Set(3,2,n[3].y());
		mat.Set(3,3,n[3].z());
		mat.Set(3,4,1.0);

		mat.Set(4,1,n[1].x());
		mat.Set(4,2,n[1].y());
		mat.Set(4,3,n[1].z());
		mat.Set(4,4,1.0);

		vec.Set(1,1,n[0].x()*p[1]->x()+n[0].y()*p[1]->y()+n[0].z()*p[1]->z());
		vec.Set(2,1,n[2].x()*p[1]->x()+n[2].y()*p[1]->y()+n[2].z()*p[1]->z());
		vec.Set(3,1,n[3].x()*p[1]->x()+n[3].y()*p[1]->y()+n[3].z()*p[1]->z());
		vec.Set(4,1,n[1].x()*p[0]->x()+n[1].y()*p[0]->y()+n[1].z()*p[0]->z());

		if(mat.Invert()==YSOK)
		{
			vec=mat*vec;
			cen.Set(vec.v(1,1),vec.v(2,1),vec.v(3,1));
			rad=vec.v(4,1);
			return YSOK;
		}
	} */
	return YSERR;
}

YSRESULT YsGetInscribedSphere(YsVec3 &cen,double &rad,const YsVec3 p[4])
{
	YsVec3 const *pp[4];
	pp[0]=&p[0];
	pp[1]=&p[1];
	pp[2]=&p[2];
	pp[3]=&p[3];
	return YsGetInscribedSphere(cen,rad,pp);
}

YSRESULT YsGetInscribedSphereRadius(double &rad,const YsVec3 p[4])
{
	YsVec3 const *pp[4];
	pp[0]=&p[0];
	pp[1]=&p[1];
	pp[2]=&p[2];
	pp[3]=&p[3];
	return YsGetInscribedSphereRadius(rad,pp);
}

////////////////////////////////////////////////////////////

static YSRESULT YsCreatePlaneCoefficients(double &a,double &b,double &c,double &d,const YsVec3 &p1,const YsVec3 &p2)
{
	YsVec3 v,m;

	v=p2-p1;
	m=(p1+p2)/2.0;

	a=v.x();
	b=v.y();
	c=v.z();

	// a*m.x()+b*m.y()+c*m.z()+d=0
	d=-(a*m.x()+b*m.y()+c*m.z());

	return YSOK;
}

YSRESULT YsGetCircumSphereOfTriangle(YsVec3 &cen,double &rad,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3)
{
	// 1st plane is on which three points are lying
	// 2nd plane intersects with p1-p2 at (p1+p2)/2 and perpendicular to p1-p2
	// 3rd plane intersects with p1-p3 at (p1+p3)/2 and perpendicular to p1-p3

	YsVec3 nom;
	double a,b,c,d;
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> rhs,lhs;

	//  static YsMatrix mat,lhs,rhs;
	//  mat.CreateWithoutClear(3,3);
	//  rhs.CreateWithoutClear(3,1);
	//  lhs.CreateWithoutClear(3,1);

	if(YsComputeNormalOfTriangle(nom,p1,p2,p3)==YSOK)
	{
		a=nom.x();
		b=nom.y();
		c=nom.z();
		d=-(a*p1.x()+b*p1.y()+c*p1.z());

		mat.Set(1,1,a);
		mat.Set(1,2,b);
		mat.Set(1,3,c);
		rhs.Set(1,1,-d);

		YsCreatePlaneCoefficients(a,b,c,d,p1,p2);
		mat.Set(2,1,a);
		mat.Set(2,2,b);
		mat.Set(2,3,c);
		rhs.Set(2,1,-d);

		YsCreatePlaneCoefficients(a,b,c,d,p1,p3);
		mat.Set(3,1,a);
		mat.Set(3,2,b);
		mat.Set(3,3,c);
		rhs.Set(3,1,-d);

		if(mat.Invert()==YSOK)
		{
			lhs=mat*rhs;
			cen.Set(lhs.v(1,1),lhs.v(2,1),lhs.v(3,1));
			rad=(p1-cen).GetLength();
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsGetInscribedSphereOfTriangle
    (YsVec3 &cen,double &rad,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3)
{
	YsVec3 o1,n1,o2,n2,e1,e2,c1,c2;

	o1=p1;
	e1=p2-p1;
	e2=p3-p1;
	if(e1.Normalize()==YSOK && e2.Normalize()==YSOK)
	{
		n1=e1+e2;

		o2=p2;
		e1=p1-p2;
		e2=p3-p2;
		if(e1.Normalize()==YSOK && e2.Normalize()==YSOK)
		{
			n2=e1+e2;

			if(n1.Normalize()==YSOK && n2.Normalize()==YSOK)
			{
				if(YsGetNearestPointOfTwoLine(c1,c2,o1,o1+n1,o2,o2+n2)==YSOK &&
				   c1==c2)
				{
					YsVec3 np;
					if(YsGetNearestPointOnLine3(np,p1,p2,c1)==YSOK)
					{
						cen=c1;
						rad=(np-c1).GetLength();
						return YSOK;
					}
				}
			}
		}
	}
	return YSERR;

}

////////////////////////////////////////////////////////////

YSRESULT YsInflateBoundingBox
    (YsVec3 &newBbxMin,YsVec3 &newBbxMax,const YsVec3 &bbxMin,const YsVec3 &bbxMax,const double ratio)
{
	YsVec3 cen;
	cen=(bbxMin+bbxMax)/2.0;
	newBbxMin=cen+(bbxMin-cen)*ratio;
	newBbxMax=cen+(bbxMax-cen)*ratio;
	return YSOK;
}


////////////////////////////////////////////////////////////

// See research note 2001/09/10
YSRESULT YsComputeTwoSphereIntersection
(YsVec3 &o,YsVec3 &n,double &r,const YsVec3 &c1,const double &r1,const YsVec3 &c2,const double &r2)
{
	double d,d1;

	d=(c2-c1).GetLength();
	d1=(d*d+r1*r1-r2*r2)/(2.0*d);

	if(d+r1<r2 || d+r2<r1)
	{
		// A sphere is completely enclosed by another.
		return YSERR;
	}

	if(r1>d1)
	{
		n=c2-c1;
		if(n.Normalize()==YSOK)
		{
			r=sqrt(r1*r1-d1*d1);
			o=c1+d1*n;
			return YSOK;
		}
	}

	return YSERR;
}

// See research note 2001/09/10
YSRESULT YsComputeThreeSphereIntersection
(YsVec3 &i1,YsVec3 &i2,
 const YsVec3 &c1,const double &r1,const YsVec3 &c2,const double &r2,const YsVec3 &c3,const double &r3)
{
	YsVec3 o1,n1,o2,n2,o3,n3;
	double rr1,rr2,rr3;

	if(YsComputeTwoSphereIntersection(o1,n1,rr1,c1,r1,c2,r2)==YSOK &&
	   YsComputeTwoSphereIntersection(o2,n2,rr2,c1,r1,c3,r3)==YSOK &&
	   YsComputeTwoSphereIntersection(o3,n3,rr3,c2,r2,c3,r3)==YSOK)
	{
		YsVec3 lo,lv;
		YsPlane p1(o1,n1),p2(o2,n2);
		if(YsGetTwoPlaneCrossLine(lo,lv,p1,p2)==YSOK)
		{
			YsPlane p3(o3,n3);
			if(lv.Normalize()==YSOK &&
			   p3.CheckOnPlane(lo)==YSTRUE &&
			   YsAbs(lv*n3)<YsTolerance)
			{
				double t1,t2,det;
				YsVec3 d;

				d=(lo-c1);
				det=YsSqr(lv*d)-(lv*lv)*(d*d-r1*r1);
				if(det>=0.0)
				{
					det=sqrt(det);
					t1=(-lv*d+det)/(lv*lv);
					t2=(-lv*d-det)/(lv*lv);

					i1=lo+t1*lv;
					i2=lo+t2*lv;
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}


YSRESULT YsComputeSphereLineIntersection(YsVec3 &i1,YsVec3 &i2,const YsVec3 &c,double r,const YsVec3 &p0,const YsVec3 &p1)
{
	auto v=p1-p0;

	const double A=v*v;
	const double B=2.0*(p0*v-v*c);
	const double C=p0*p0+c*c-2.0*p0*c-r*r;

	auto D=B*B-4.0*A*C;
	if(0.0<D)
	{
		auto s=sqrt(D);
		auto t0=(-B+s)/(2.0*A);
		auto t1=(-B+s)/(2.0*A);
		i1=p0+v*t0;
		i2=p0+v*t1;
		return YSOK;
	}
	return YSERR;
}


YSRESULT YsGetNearestPointOnLineSegments3
   (YsVec3 &nearPos,int &nearIdx,YSBOOL &nearNode,YSSIZE_T np,const YsVec3 p[],const YsVec3 &refp)
{
	double nearNodeDistSq=0.0,nearEdgeDistSq=0.0;
	int i,nearNodeIdx,nearEdgeIdx;
	YsVec3 nearEdgePos;
	if(np>0)
	{
		nearNodeIdx=-1;
		nearEdgeIdx=-1;

		for(i=0; i<np; i++)
		{
			double l;
			l=(p[i]-refp).GetSquareLength();
			if(nearNodeIdx<0 || l<nearNodeDistSq)
			{
				nearNodeDistSq=l;
				nearNodeIdx=i;
			}
		}

		YsVec3 const *edVtPos[2];
		edVtPos[1]=&p[0];
		for(i=1; i<np; i++)
		{
			edVtPos[0]=edVtPos[1];
			edVtPos[1]=&p[i];

			YsVec3 nea;
			if(YsGetNearestPointOnLine3(nea,(*edVtPos[0]),(*edVtPos[1]),refp)==YSOK &&
			   YsCheckInBetween3(nea,(*edVtPos[0]),(*edVtPos[1]))==YSTRUE)
			{
				double l;
				l=(nea-refp).GetSquareLength();
				if(nearEdgeIdx<0 || l<nearEdgeDistSq)
				{
					nearEdgeDistSq=l;
					nearEdgeIdx=i-1;
					nearEdgePos=nea;
				}
			}
		}

		if(nearEdgeIdx>=0 && nearEdgeDistSq<nearNodeDistSq)
		{
			nearPos=nearEdgePos;
			nearNode=YSFALSE;
			nearIdx=nearEdgeIdx;
			return YSOK;
		}
		else
		{
			nearPos=p[nearNodeIdx];
			nearNode=YSTRUE;
			nearIdx=nearNodeIdx;
			return YSOK;
		}
	}
	nearPos=YsOrigin();
	nearNode=YSFALSE;
	nearIdx=-1;
	return YSERR;
}

YSRESULT YsGetNearestPointOnLineSegments2
   (YsVec2 &nearPos,int &nearIdx,YSBOOL &nearNode,YSSIZE_T np,const YsVec2 p[],const YsVec2 &refp)
{
	double nearNodeDistSq=0.0,nearEdgeDistSq=0.0;
	int i,nearNodeIdx,nearEdgeIdx;
	YsVec2 nearEdgePos;
	if(np>0)
	{
		nearNodeIdx=-1;
		nearEdgeIdx=-1;

		for(i=0; i<np; i++)
		{
			double l;
			l=(p[i]-refp).GetSquareLength();
			if(nearNodeIdx<0 || l<nearNodeDistSq)
			{
				nearNodeDistSq=l;
				nearNodeIdx=i;
			}
		}

		YsVec2 const *edVtPos[2];
		edVtPos[1]=&p[0];
		for(i=1; i<np; i++)
		{
			edVtPos[0]=edVtPos[1];
			edVtPos[1]=&p[i];

			YsVec2 nea;
			if(YsGetNearestPointOnLine2(nea,(*edVtPos[0]),(*edVtPos[1]),refp)==YSOK &&
			   YsCheckInBetween2(nea,(*edVtPos[0]),(*edVtPos[1]))==YSTRUE)
			{
				double l;
				l=(nea-refp).GetSquareLength();
				if(nearEdgeIdx<0 || l<nearEdgeDistSq)
				{
					nearEdgeDistSq=l;
					nearEdgeIdx=i-1;
					nearEdgePos=nea;
				}
			}
		}

		if(nearEdgeIdx>=0 && nearEdgeDistSq<nearNodeDistSq)
		{
			nearPos=nearEdgePos;
			nearNode=YSFALSE;
			nearIdx=nearEdgeIdx;
			return YSOK;
		}
		else
		{
			nearPos=p[nearNodeIdx];
			nearNode=YSTRUE;
			nearIdx=nearNodeIdx;
			return YSOK;
		}
	}
	nearPos.Set(0.0,0.0);
	nearNode=YSFALSE;
	nearIdx=-1;
	return YSERR;
}

YSRESULT YsGetTangentialVectorAtNodeOfLineSegments3(YsVec3 &unitTan,YSSIZE_T ndId,YSSIZE_T np,const YsVec3 p[])
{
	if(0<=ndId && ndId<np)
	{
		YSSIZE_T i;
		YsVec3 q[2];
		q[0]=p[ndId];
		q[1]=p[ndId];
		for(i=ndId-1; i>=0; i--)
		{
			if(p[i]!=q[0])
			{
				q[0]=p[i];
				break;
			}
		}
		for(i=ndId+1; i<np; i++)
		{
			if(p[i]!=q[1])
			{
				q[1]=p[i];
				break;
			}
		}

		unitTan=q[1]-q[0];
		return unitTan.Normalize();
	}
	unitTan=YsOrigin();
	return YSERR;
}

YSRESULT YsGetTangentialVectorOnEdgeOfLineSegments3(YsVec3 &unitTan,int edId,int np,const YsVec3 p[])
{
	if(0<=edId && edId<np-1)
	{
		int i;
		YsVec3 q[2];
		q[0]=p[edId];
		q[1]=p[edId];
		for(i=edId+1; i<np; i++)
		{
			if(p[i]!=q[1])
			{
				q[1]=p[i];
				break;
			}
		}
		unitTan=q[1]-q[0];
		return unitTan.Normalize();
	}
	unitTan=YsOrigin();
	return YSERR;
}


////////////////////////////////////////////////////////////

YSRESULT YsFindLeastSquareFittingPlane(double &aa,double &bb,double &cc,double &dd,YSSIZE_T nVtx,const YsVec3 vtx[])
{
	// ax+by+cz+d=0
	double a,b,c,d,l;
	int i,j,nSol;
	double aSol[3],bSol[3],cSol[3],dSol[3];
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> rhs,sol;

	nSol=0;

	// Assuming c==1
	mat.Set(1,1,0.0);
	mat.Set(1,2,0.0);
	mat.Set(1,3,0.0);
	mat.Set(2,1,0.0);
	mat.Set(2,2,0.0);
	mat.Set(2,3,0.0);
	mat.Set(3,1,0.0);
	mat.Set(3,2,0.0);
	mat.Set(3,3,0.0);
	rhs.Set(1,1,0.0);
	rhs.Set(2,1,0.0);
	rhs.Set(3,1,0.0);
	for(i=0; i<nVtx; i++)
	{
		mat.Set(1,1,mat.v(1,1)+YsSqr(vtx[i].x()));
		mat.Set(1,2,mat.v(1,2)+vtx[i].x()*vtx[i].y());
		mat.Set(1,3,mat.v(1,3)+vtx[i].x());
		rhs.Set(1,1,rhs.v(1,1)-vtx[i].x()*vtx[i].z());

		mat.Set(2,1,mat.v(2,1)+vtx[i].x()*vtx[i].y());
		mat.Set(2,2,mat.v(2,2)+YsSqr(vtx[i].y()));
		mat.Set(2,3,mat.v(2,3)+vtx[i].y());
		rhs.Set(2,1,rhs.v(2,1)-vtx[i].y()*vtx[i].z());

		mat.Set(3,1,mat.v(3,1)+vtx[i].x());
		mat.Set(3,2,mat.v(3,2)+vtx[i].y());
		mat.Set(3,3,mat.v(3,3)+1.0);
		rhs.Set(3,1,rhs.v(3,1)-vtx[i].z());
	}

	YsBalanceEquation <3> (mat,rhs);

	if(mat.Invert()==YSOK)
	{
		sol=mat*rhs;
		aSol[nSol]=sol.v(1,1);
		bSol[nSol]=sol.v(2,1);
		cSol[nSol]=1.0;
		dSol[nSol]=sol.v(3,1);
		nSol++;
	}


	// Assuming b==1
	mat.Set(1,1,0.0);
	mat.Set(1,2,0.0);
	mat.Set(1,3,0.0);
	mat.Set(2,1,0.0);
	mat.Set(2,2,0.0);
	mat.Set(2,3,0.0);
	mat.Set(3,1,0.0);
	mat.Set(3,2,0.0);
	mat.Set(3,3,0.0);
	rhs.Set(1,1,0.0);
	rhs.Set(2,1,0.0);
	rhs.Set(3,1,0.0);
	for(i=0; i<nVtx; i++)
	{
		mat.Set(1,1,mat.v(1,1)+YsSqr(vtx[i].x()));
		mat.Set(1,2,mat.v(1,2)+vtx[i].x()*vtx[i].z());
		mat.Set(1,3,mat.v(1,3)+vtx[i].x());
		rhs.Set(1,1,rhs.v(1,1)-vtx[i].x()*vtx[i].y());

		mat.Set(2,1,mat.v(2,1)+vtx[i].x()*vtx[i].z());
		mat.Set(2,2,mat.v(2,2)+YsSqr(vtx[i].z()));
		mat.Set(2,3,mat.v(2,3)+vtx[i].z());
		rhs.Set(2,1,rhs.v(2,1)-vtx[i].z()*vtx[i].y());

		mat.Set(3,1,mat.v(3,1)+vtx[i].x());
		mat.Set(3,2,mat.v(3,2)+vtx[i].z());
		mat.Set(3,3,mat.v(3,3)+1.0);
		rhs.Set(3,1,rhs.v(3,1)-vtx[i].y());
	}

	YsBalanceEquation <3> (mat,rhs);

	if(mat.Invert()==YSOK)
	{
		sol=mat*rhs;
		aSol[nSol]=sol.v(1,1);
		bSol[nSol]=1.0;
		cSol[nSol]=sol.v(2,1);
		dSol[nSol]=sol.v(3,1);
		nSol++;
	}



	// Assuming a==1
	mat.Set(1,1,0.0);
	mat.Set(1,2,0.0);
	mat.Set(1,3,0.0);
	mat.Set(2,1,0.0);
	mat.Set(2,2,0.0);
	mat.Set(2,3,0.0);
	mat.Set(3,1,0.0);
	mat.Set(3,2,0.0);
	mat.Set(3,3,0.0);
	rhs.Set(1,1,0.0);
	rhs.Set(2,1,0.0);
	rhs.Set(3,1,0.0);
	for(i=0; i<nVtx; i++)
	{
		mat.Set(1,1,mat.v(1,1)+YsSqr(vtx[i].y()));
		mat.Set(1,2,mat.v(1,2)+vtx[i].y()*vtx[i].z());
		mat.Set(1,3,mat.v(1,3)+vtx[i].y());
		rhs.Set(1,1,rhs.v(1,1)-vtx[i].y()*vtx[i].x());

		mat.Set(2,1,mat.v(2,1)+vtx[i].y()*vtx[i].z());
		mat.Set(2,2,mat.v(2,2)+YsSqr(vtx[i].z()));
		mat.Set(2,3,mat.v(2,3)+vtx[i].z());
		rhs.Set(2,1,rhs.v(2,1)-vtx[i].x()*vtx[i].z());

		mat.Set(3,1,mat.v(3,1)+vtx[i].y());
		mat.Set(3,2,mat.v(3,2)+vtx[i].z());
		mat.Set(3,3,mat.v(3,3)+1.0);
		rhs.Set(3,1,rhs.v(3,1)-vtx[i].x());
	}

	YsBalanceEquation <3> (mat,rhs);

	if(mat.Invert()==YSOK)
	{
		sol=mat*rhs;
		aSol[nSol]=1.0;
		bSol[nSol]=sol.v(1,1);
		cSol[nSol]=sol.v(2,1);
		dSol[nSol]=sol.v(3,1);
		nSol++;
	}

	int choice=-1;
	double least,sq;
	least=YsInfinity;
	for(i=0; i<nSol; i++)
	{
		a=aSol[i];
		b=bSol[i];
		c=cSol[i];
		d=dSol[i];

		l=sqrt(a*a+b*b+c*c);
		if(l>=YsTolerance)  // 2009/03/25
		{
			a/=l;
			b/=l;
			c/=l;
			d/=l;

			aSol[i]=a;
			bSol[i]=b;
			cSol[i]=c;
			dSol[i]=d;

			sq=0.0;
			for(j=0; j<nVtx; j++)
			{
				// sq+=YsSqr(a*vtx[j].x()+b*vtx[j].y()+c*vtx[j].z()+d);
				sq=YsGreater(sq,YsSqr(a*vtx[j].x()+b*vtx[j].y()+c*vtx[j].z()+d));
			}

			// printf("[%d] %.2lf %.2lf %.2lf %.2lf sq=%.2lf\n",i,a,b,c,d,sq);

			if(sq<least)
			{
				choice=i;
				least=sq;
			}
		}
	}

	if(choice>=0)
	{
		aa=aSol[choice];
		bb=bSol[choice];
		cc=cSol[choice];
		dd=dSol[choice];
		return YSOK;
	}

	return YSERR;
}

YSRESULT YsFindLeastSquareFittingPlaneNormal(YsVec3 &nom,YSSIZE_T np,const YsVec3 p[])
{
	double a,b,c,d;
	if(YsFindLeastSquareFittingPlane(a,b,c,d,np,p)==YSOK)
	{
		nom.Set(a,b,c);
		return nom.Normalize();
	}
	nom=YsOrigin();
	return YSERR;
}

YSRESULT YsFindLeastSquareFittingPlane(YsVec3 &org,YsVec3 &nom,YSSIZE_T np,const YsVec3 p[])
{
	double a,b,c,d;
	if(YsFindLeastSquareFittingPlane(a,b,c,d,np,p)==YSOK)
	{
		YSSIZE_T i;
		double t,numer,denom;
		YsVec3 ref;

		nom.Set(a,b,c);

		ref=p[0];
		for(i=1; i<np; i++)
		{
			ref+=p[i];
		}
		ref/=(double)np;

		numer=-ref*nom-d;
		denom=nom.GetSquareLength();
		if(denom>YsTolerance)
		{
			t=numer/denom;
			org=ref+t*nom;

			return nom.Normalize();
		}
	}
	return YSERR;
}



YSRESULT YsFindLeastSquarePointFromLine(YsVec3 &pos,const YSSIZE_T nLine,const YsVec3 o[],const YsVec3 _v[])
{
	YSSIZE_T i;
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> rhs,sol;

	mat.Set(1,1,0.0);
	mat.Set(1,2,0.0);
	mat.Set(1,3,0.0);
	mat.Set(2,1,0.0);
	mat.Set(2,2,0.0);
	mat.Set(2,3,0.0);
	mat.Set(3,1,0.0);
	mat.Set(3,2,0.0);
	mat.Set(3,3,0.0);
	rhs.Set(1,1,0.0);
	rhs.Set(2,1,0.0);
	rhs.Set(3,1,0.0);

	for(i=0; i<nLine; i++)
	{
		YsVec3 v;
		v=_v[i];
		if(v.Normalize()==YSOK)
		{
			const double &ox=o[i].x();
			const double &oy=o[i].y();
			const double &oz=o[i].z();
			const double &vx=v.x();
			const double &vy=v.y();
			const double &vz=v.z();
			double vv,vo,vxvx,vyvy,vzvz,vxvy,vxvz,vyvz;

			vv=v*v;
			vo=v*o[i];

			vxvx=v.x()*v.x();
			vyvy=v.y()*v.y();
			vzvz=v.z()*v.z();

			vxvy=v.x()*v.y();
			vxvz=v.x()*v.z();
			vyvz=v.y()*v.z();

			mat.Set(1,1,mat.v(1,1)+2.0*(1.0-2.0*vxvx+vv*vxvx));
			mat.Set(1,2,mat.v(1,2)-4.0*vxvy+2.0*vv*vxvy);
			mat.Set(1,3,mat.v(1,3)-4.0*vxvz+2.0*vv*vxvz);
			rhs.Set(1,1,rhs.v(1,1)-2.0*(-ox+2.0*vo*vx-vv*vo*vx));

			mat.Set(2,1,mat.v(2,1)-4.0*vxvy+2.0*vv*vxvy);
			mat.Set(2,2,mat.v(2,2)+2.0*(1.0-2.0*vyvy+vv*vyvy));
			mat.Set(2,3,mat.v(2,3)-4.0*vyvz+2.0*vv*vyvz);
			rhs.Set(2,1,rhs.v(2,1)-2.0*(-oy+2.0*vo*vy-vv*vo*vy));

			mat.Set(3,1,mat.v(3,1)-4.0*vxvz+2.0*vv*vxvz);
			mat.Set(3,2,mat.v(3,2)-4.0*vyvz+2.0*vv*vyvz);
			mat.Set(3,3,mat.v(3,3)+2.0*(1.0-2.0*vzvz+vv*vzvz));
			rhs.Set(3,1,rhs.v(3,1)-2.0*(-oz+2.0*vo*vz-vv*vo*vz));
		}
	}

	YsBalanceEquation <3> (mat,rhs);

	if(mat.Invert()==YSOK)
	{
		sol=mat*rhs;
		pos.Set(sol.v(1,1),sol.v(2,1),sol.v(3,1));
		return YSOK;
	}

	return YSERR;
}

YsResultAndVec3 YsFindLeastSquarePoint(YsConstArrayMask <YsPlane> pln,YsConstArrayMask <YsLine3> line)
{
	YsMatrixTemplate <3,3> mat;
	YsMatrixTemplate <3,1> rhs,sol;

	mat.Set(1,1,0.0);
	mat.Set(1,2,0.0);
	mat.Set(1,3,0.0);
	mat.Set(2,1,0.0);
	mat.Set(2,2,0.0);
	mat.Set(2,3,0.0);
	mat.Set(3,1,0.0);
	mat.Set(3,2,0.0);
	mat.Set(3,3,0.0);
	rhs.Set(1,1,0.0);
	rhs.Set(2,1,0.0);
	rhs.Set(3,1,0.0);

	for(auto &l : line)
	{
		auto o=l.Origin();
		auto v=l.Vector();

		const double &ox=o.x();
		const double &oy=o.y();
		const double &oz=o.z();
		const double &vx=v.x();
		const double &vy=v.y();
		const double &vz=v.z();
		double vv,vo,vxvx,vyvy,vzvz,vxvy,vxvz,vyvz;

		vv=v*v;
		vo=v*o;

		vxvx=v.x()*v.x();
		vyvy=v.y()*v.y();
		vzvz=v.z()*v.z();

		vxvy=v.x()*v.y();
		vxvz=v.x()*v.z();
		vyvz=v.y()*v.z();

		mat.Add(1,1,+2.0*(1.0-2.0*vxvx+vv*vxvx));
		mat.Add(1,2,-4.0*vxvy+2.0*vv*vxvy);
		mat.Add(1,3,-4.0*vxvz+2.0*vv*vxvz);
		rhs.Add(1,1,-2.0*(-ox+2.0*vo*vx-vv*vo*vx));

		mat.Add(2,1,-4.0*vxvy+2.0*vv*vxvy);
		mat.Add(2,2,+2.0*(1.0-2.0*vyvy+vv*vyvy));
		mat.Add(2,3,-4.0*vyvz+2.0*vv*vyvz);
		rhs.Add(2,1,-2.0*(-oy+2.0*vo*vy-vv*vo*vy));

		mat.Add(3,1,-4.0*vxvz+2.0*vv*vxvz);
		mat.Add(3,2,-4.0*vyvz+2.0*vv*vyvz);
		mat.Add(3,3,+2.0*(1.0-2.0*vzvz+vv*vzvz));
		rhs.Add(3,1,-2.0*(-oz+2.0*vo*vz-vv*vo*vz));
	}

	for(auto &p : pln)
	{
		double on=p.GetOrigin()*p.GetNormal();
		double nx=p.GetNormal().x();
		double ny=p.GetNormal().y();
		double nz=p.GetNormal().z();

		mat.Add(1,1,nx*nx);
		mat.Add(1,2,nx*ny);
		mat.Add(1,3,nx*nz);

		mat.Add(2,1,nx*ny);
		mat.Add(2,2,ny*ny);
		mat.Add(2,3,ny*nz);

		mat.Add(3,1,nx*nz);
		mat.Add(3,2,ny*nz);
		mat.Add(3,3,nz*nz);

		rhs.Add(1,1,nx*on);
		rhs.Add(2,1,ny*on);
		rhs.Add(3,1,nz*on);
	}

	YsBalanceEquation <3> (mat,rhs);

	YsResultAndVec3 respos;
	respos.res=mat.Invert();
	if(YSOK==respos.res)
	{
		sol=mat*rhs;
		respos.pos.Set(sol.v(1,1),sol.v(2,1),sol.v(3,1));
		respos.res=YSOK;
	}
	else
	{
		respos.pos=YsVec3::Origin();
	}

	return respos;
}

double YsCalculateTotalLength2(YSSIZE_T np,const YsVec2 p[],YSBOOL isLoop)
{
	int i;
	double l;
	l=0.0;
	for(i=0; i<np-1; i++)
	{
		l+=(p[i+1]-p[i]).GetLength();
	}
	if(isLoop==YSTRUE)
	{
		l+=(p[np-1]-p[0]).GetLength();
	}
	return l;
}

double YsCalculateTotalLength3(YSSIZE_T np,const YsVec3 p[],YSBOOL isLoop)
{
	int i;
	double l;
	l=0.0;
	for(i=0; i<np-1; i++)
	{
		l+=(p[i+1]-p[i]).GetLength();
	}
	if(isLoop==YSTRUE)
	{
		l+=(p[np-1]-p[0]).GetLength();
	}
	return l;
}

double YsCalculateQuadWarpage(const YsVec3 p[4])
{
	double maxWarpCos=1.0;
	for(int i=0; i<2; ++i)
	{
		// 3 2
		// 0 1
		YsVec3 v1=p[(i+1)%4]-p[i];
		YsVec3 v2=p[(i+2)%4]-p[i];
		YsVec3 v3=p[(i+3)%4]-p[i];

		v1.Normalize();
		v2.Normalize();
		v3.Normalize();

		YsVec3 n1=(v1^v2);
		YsVec3 n2=(v2^v3);

		n1.Normalize();
		n2.Normalize();

		maxWarpCos=YsSmaller(maxWarpCos,n1*n2);
	}
	return maxWarpCos;
}

double YsCalculateCounterClockwiseAngle(const YsVec3 &tstVec,const YsVec3 &refVec,const YsVec3 &unitClockFaceDir)
{
	const YsVec3 unitRefVec=YsUnitVector(refVec-unitClockFaceDir*(unitClockFaceDir*refVec));

	const YsVec3 unitTstVec=YsUnitVector(tstVec-unitClockFaceDir*(unitClockFaceDir*tstVec));

	const double dotProd=YsBound(unitRefVec*unitTstVec,-1.0,1.0);
	const YsVec3 crsProd=unitRefVec^unitTstVec;

	if(0.0<=crsProd*unitClockFaceDir)
	{
		return acos(dotProd);
	}
	else
	{
		return YsPi*2.0-acos(dotProd);
	}
}

void YsCalculateLatticeDivision3(int &nx,int &ny,int &nz,YsVec3 *unitBox,const YsVec3 bbx[2],int nLatticeCell)
{
	YsCalculateLatticeDivision3(nx,ny,nz,unitBox,bbx[0],bbx[1],nLatticeCell);
}

void YsCalculateLatticeDivision3(int &nx,int &ny,int &nz,YsVec3 *unitBox,const YsVec3 &bbx0,const YsVec3 &bbx1,int nLatticeCell)
{
	if(1>nLatticeCell)
	{
		nLatticeCell=1;
	}

	YsBoundingBoxMaker3 mkBbx;
	mkBbx.Add(bbx0);
	mkBbx.Add(bbx1);

	YsVec3 min,max;
	mkBbx.Get(min,max);

	const YsVec3 dgn=max-min;

	const double unitVol=(dgn.x()*dgn.y()*dgn.z())/(double)nLatticeCell;

	const double unitD=pow(unitVol,1.0/3.0);

	nx=YsGreater<int>(1,(int)(dgn.x()/unitD));
	ny=YsGreater<int>(1,(int)(dgn.y()/unitD));
	nz=YsGreater<int>(1,(int)(dgn.z()/unitD));

	if(NULL!=unitBox)
	{
		(*unitBox)=dgn;
		unitBox->DivX((double)nx);
		unitBox->DivY((double)ny);
		unitBox->DivZ((double)nz);
	}
}

YSRESULT YsComputeCircleLineIntersection(YsVec2 itsc[2],const YsVec2 &cen,const double rad,const YsVec2 &p0,const YsVec2 &p1)
{
	// itsc=p0+t*v

	// (p0+t*v-cen)^2=rad^2
	// p0^2 + t^2*v^2 + cen^2 + 2.0t*p0*v -2.0*p0*cen -2.0*t*v*cen-rad^2=0
	// t^2*(v^2)+2.0*t(p0*v-v*cen)+p0^2+cen^2-2.0*p0*cen-rad^2=0

	YsVec2 v=p1-p0;
	if(YSOK==v.Normalize())
	{
		const double a=1.0; // v*v;
		const double b=p0*v-v*cen;
		const double c=p0*p0+cen*cen-2.0*p0*cen-rad*rad;

		// x=(-b+-sqrt(b*b-ac))/a
		const double D=b*b-a*c;
		if(0.0<=D)
		{
			const double sqrtD=sqrt(D);
			const double t[2]=
			{
				(-b+sqrtD)/a,
				(-b-sqrtD)/a,
			};

			itsc[0]=p0+t[0]*v;
			itsc[1]=p0+t[1]*v;

			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsComputeCircleLineIntersection(YsVec2 itsc[2],const YsVec2 &cen,const double rad,const YsVec2 p[2])
{
	return YsComputeCircleLineIntersection(itsc,cen,rad,p[0],p[1]);
}

YSRESULT YsComputeCircleTangentPoint(YsVec2 tanPos[2],const YsVec2 &cen,const double r,const YsVec2 &passingPoint)
{
	const double d=(passingPoint-cen).GetLength();
	const double D=d*d-r*r;
	if(0.0<=D && YsTolerance<d)
	{
		const YsVec2 Uop=(passingPoint-cen)/d;

		const double L=sqrt(D);
		const double x=(r*r)/d;
		const double y=(r*L)/d;

		const YsVec2 foot=cen+Uop*x;

		const YsVec2 Y(-Uop.y(),Uop.x());
		tanPos[0]=foot+y*Y;
		tanPos[1]=foot-y*Y;
		return YSOK;
	}
	return YSERR;
}


YSBOOL YsIsConvexCorner2(YSSIZE_T np,const YsVec2 p[],YSSIZE_T cornerIdx)
{
	double totalAngle=0.0;
	double signAtCornerIdx=0.0;

	// First loop: Finding the last non-zero edge vector.
	YsVec2 prvVec;
	for(YSSIZE_T idx=np-1; 0<=idx; --idx)
	{
		prvVec=p[(idx+1)%np]-p[idx];
		if(YSOK==prvVec.Normalize())
		{
			break;
		}
	}

	// Second loop: Calculate total winding.  This must be either +360 or -360 degrees.
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		YsVec2 v0=prvVec;
		YsVec2 v1=p[(idx+1)%np]-p[idx];

		if(YSOK!=v1.Normalize())
		{
			continue;  // Do not even update the prvVec if the edge vector is zero.
		}

		const double crs=v0^v1;

		if(idx==cornerIdx)
		{
			signAtCornerIdx=crs;
			if(YsTolerance>fabs(signAtCornerIdx))
			{
				return YSTFUNKNOWN;  // The corner is either no bending, a 180-degree folding, or a collpased edge.
			}
		}

		if(YsTolerance>fabs(crs))
		{
			continue;  // Do not even update the prvVec if 180-degree folding.
		}
		else 
		{
			const double dot=YsBound(v0*v1,-1.0,1.0);
			if(0.0<crs)
			{
				totalAngle+=acos(dot);
			}
			else
			{
				totalAngle-=acos(dot);
			}
		}

		prvVec=v1;
	}

	if(YsTolerance<fabs(signAtCornerIdx))
	{
		YSBOOL res=(0.0<signAtCornerIdx ? YSTRUE : YSFALSE);
		if(0.0>totalAngle)
		{
			YsFlip(res);
		}
		return res;
	}
	return YSTFUNKNOWN;
}

YSBOOL YsIsConvexCorner3(YSSIZE_T np,const YsVec3 p[],YSSIZE_T cornerIdx)
{
	YsMatrix3x3 toXY;
	const YsVec3 nom=YsGetAverageNormalVector(np,p);
	toXY.MakeToXY(nom);

	YsArray <YsVec2,8> prj(np,NULL);
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		prj[idx]=(toXY*p[idx]).xy();
	}

	return YsIsConvexCorner2(np,prj,cornerIdx);
}

int YsFindClosestAxis(const YsVec3 &dir,YSSIZE_T nAxis,const YsVec3 axis[])
{
	int minIndex=0;
	if(0<nAxis)
	{
		double minCos=fabs(axis[0]*dir);
		for(int index=0; index<nAxis; ++index)
		{
			const double c=fabs(axis[index]*dir);
			if(minCos<c)
			{
				minIndex=index;
				minCos=c;
			}
		}
	}
	return minIndex;
}

int YsFindClosestVector(const YsVec3 &dir,YSSIZE_T nVec,const YsVec3 vec[])
{
	int minIndex=0;
	if(0<nVec)
	{
		double minCos=vec[0]*dir;
		for(int index=0; index<nVec; ++index)
		{
			const double c=vec[index]*dir;
			if(minCos<c)
			{
				minIndex=index;
				minCos=c;
			}
		}
	}
	return minIndex;
}

/*! This function calculates an intersection between a cylinder and a line.
    The centroid of the cylinder passes through the origin and is parallel to vc, and the line passes through two points p1 and p2. */
static YSRESULT YsCalculateCylinderLineIntersection(YsVec3 itsc[2],const YsVec3 &vc,const double radius,const YsVec3 &p1,const YsVec3 &p2)
{
	const YsVec3 &Oa=p1;
	const YsVec3 Va=YsUnitVector(p2-p1);

	const YsVec3 Oc=YsOrigin();
	const YsVec3 Vc=YsUnitVector(vc);

	const YsVec3 Vca=Oa-Oc;

	const double VaVc=Va*Vc;
	const double VcaVa=Vca*Va;
	const double VcaVc=Vca*Vc;
	const double Va2=Va*Va;
	const double Vc2=Vc*Vc;
	const double Vca2=Vca*Vca;
	const double VaVc2=VaVc*VaVc;
	const double VcaVc2=VcaVc*VcaVc;
	const double r2=radius*radius;

	const double a=Va2+VaVc2*Vc2-2.0*VaVc2;
	const double b=2.0*VcaVa-4.0*VaVc*VcaVc+2.0*VcaVc*VaVc*Vc2;
	const double c=Vca2+VcaVc2*Vc2-2.0*VcaVc2-r2;

	const double D=b*b-4.0*a*c;
	if(0.0<=D)
	{
		const double t0=(-b+sqrt(D))/(2.0*a);
		const double t1=(-b-sqrt(D))/(2.0*a);

		itsc[0]=Oa+t0*Va;
		itsc[1]=Oa+t1*Va;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsCalculateCylinderLineIntersection(YsVec3 itsc[2],const YsVec3 &c1,const YsVec3 &c2,const double radius,const YsVec3 &p1,const YsVec3 &p2)
{
	if(YSOK==YsCalculateCylinderLineIntersection(itsc,c2-c1,radius,p1-c1,p2-c1))
	{
		itsc[0]+=c1;
		itsc[1]+=c1;
		return YSOK;
	}
	return YSERR;
}

template <class VectorClass>
YSSIZE_T YsFindLongestEdgeIndexOfPolygonTemplate(YSSIZE_T np,const VectorClass plg[])
{
	YSSIZE_T longestIdx=0;
	if(2<=np)
	{
		double longestLSq=(plg[1]-plg[0]).GetSquareLength();
		for(YSSIZE_T idx=0; idx<np; ++idx)
		{
			const double lSq=(YsGetCyclic(np,plg,idx+1)-plg[idx]).GetSquareLength();
			if(lSq>longestLSq)
			{
				longestIdx=idx;
				longestLSq=lSq;
			}
		}
	}
	return longestIdx;
}

YSSIZE_T YsFindLongestEdgeIndexOfPolygon(YSSIZE_T np,const YsVec3 plg[])
{
	return YsFindLongestEdgeIndexOfPolygonTemplate(np,plg);
}

YSSIZE_T YsFindLongestEdgeIndexOfPolygon(YSSIZE_T np,const YsVec2 plg[])
{
	return YsFindLongestEdgeIndexOfPolygonTemplate(np,plg);
}


template <class VectorClass>
YSSIZE_T YsFindShortestEdgeIndexOfPolygonTemplate(YSSIZE_T np,const VectorClass plg[])
{
	YSSIZE_T shortestIdx=0;
	if(2<=np)
	{
		double shortestLSq=(plg[1]-plg[0]).GetSquareLength();
		for(YSSIZE_T idx=0; idx<np; ++idx)
		{
			const double lSq=(YsGetCyclic(np,plg,idx+1)-plg[idx]).GetSquareLength();
			if(lSq<shortestLSq)
			{
				shortestIdx=idx;
				shortestLSq=lSq;
			}
		}
	}
	return shortestIdx;
}

YSSIZE_T YsFindShortestEdgeIndexOfPolygon(YSSIZE_T np,const YsVec3 plg[])
{
	return YsFindShortestEdgeIndexOfPolygonTemplate(np,plg);
}

YSSIZE_T YsFindShortestEdgeIndexOfPolygon(YSSIZE_T np,const YsVec2 plg[])
{
	return YsFindShortestEdgeIndexOfPolygonTemplate(np,plg);
}


// The following pragma is needed for compiling this code with Visual C++ 2012.
// Without the pragma, the compiler dies with internal error C1001.  (Tested with Visual C++ 2012 Update 4)
#pragma optimize("g",off)
double YsGetPolygonFaceAngle(YSSIZE_T np,const YsVec2 p[],YSSIZE_T vtIdx)
{
	// See 20150327-PolygonFaceAngle.jnt

	YSSIZE_T offset=1;
	YsVec2 curDir;
	double accum=0.0;

	YsConstArrayMask <YsVec2> plVtPos(np,p);

	// First find the meaning ful direction.
	while(offset<np)
	{
		curDir=(plVtPos.GetCyclic(vtIdx+offset)-plVtPos[vtIdx]);
		if(YSOK==curDir.Normalize())
		{
			break;
		}
		++offset;
	}

	// Then accumurate all windings
	while(offset<np)
	{
		auto nextDir=(plVtPos.GetCyclic(vtIdx+offset)-plVtPos[vtIdx]);
		if(YSOK==nextDir.Normalize())
		{
			double a=acos(YsBound(curDir*nextDir,-1.0,1.0));
			if(0.0<(curDir^nextDir))
			{
				accum+=a;
			}
			else
			{
				accum-=a;
			}
			curDir=nextDir;
		}
		++offset;
	}

	return fabs(accum);
}
#pragma optimize("g",on)



////////////////////////////////////////////////////////////

static YSBOOL YsVectorPointingInsideFromPoint(YSSIZE_T np,const YsVec2 p[],YSSIZE_T vtIdx,const YsVec2 &vecIn,const YsVec2 &from)
{
	YSSIZE_T offset=1;
	YsVec2 curVec;
	double accum=0.0;

	YsConstArrayMask <YsVec2> plVtPos(np,p);

	// First find the meaning ful direction.
	while(offset<np)
	{
		curVec=(plVtPos.GetCyclic(vtIdx+offset)-from);
		if(YSOK==curVec.Normalize())
		{
			break;
		}
		++offset;
	}


	const YsVec2 refVec=YsUnitVector(vecIn);
	if(refVec==curVec)
	{
		return YSTFUNKNOWN; // On the boundary.
	}


	const int STATE_NORMAL=0;
	const int STATE_TOUCH_FROM_RIGHT=1;
	const int STATE_TOUCH_FROM_LEFT=2;

	int parity=0;
	int state=STATE_NORMAL;
	// Then calculate parity
	while(offset<=np)  // Equal sign needed for VectorPointingInsideFromEdge.  If from==p[vtIdx], Normalize() will fail and will be ignored.
	{
		auto nextVec=(plVtPos.GetCyclic(vtIdx+offset)-from);
		if(YSOK==nextVec.Normalize())
		{
			switch(state)
			{
			case STATE_NORMAL:
				if(nextVec==refVec)
				{
					if(0.0<(curVec^nextVec))
					{
						state=STATE_TOUCH_FROM_RIGHT;
					}
					else
					{
						state=STATE_TOUCH_FROM_LEFT;
					}
				}
				else if(curVec*refVec>curVec*nextVec && 0.0<(curVec^refVec)*(curVec^nextVec))
				{
					++parity;
				}
				break;
			case STATE_TOUCH_FROM_RIGHT:
				if(nextVec!=refVec)
				{
					if(0.0<(curVec^nextVec))
					{
						++parity;
					}
					state=STATE_NORMAL;
				}
				break;
			case STATE_TOUCH_FROM_LEFT:
				if(nextVec!=refVec)
				{
					if(0.0>(curVec^nextVec))
					{
						++parity;
					}
					state=STATE_NORMAL;
				}
				break;
			}
			curVec=nextVec;
		}
		++offset;
	}
	if(state!=STATE_NORMAL)
	{
		return YSTFUNKNOWN; // Parallel to the last edge.
	}

	if(0!=(parity&1))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsVectorPointingInside(YSSIZE_T np,const YsVec2 p[],YSSIZE_T vtIdx,const YsVec2 &vecIn)
{
	return YsVectorPointingInsideFromPoint(np,p,vtIdx,vecIn,p[vtIdx]);
}

YSBOOL YsVectorPointingInsideFromEdge(YSSIZE_T np,const YsVec2 p[],YSSIZE_T edIdx,const YsVec2 &vecIn)
{
	auto from=(p[edIdx]+p[(edIdx+1)%np])/2.0;
	return YsVectorPointingInsideFromPoint(np,p,edIdx,vecIn,from);
}

YSRESULT YsMakePlaneProjectionMatrix(YsMatrix4x4 &tfm,const YsVec3 &o,const YsVec3 &n,const YsVec3 &q)  // q:Direction of projection
{
	const double qn=q*n;
	const double on=o*n;

	if(fabs(qn)>YsTolerance)
	{
		tfm.Set(1,1,1.0-(n.x()*q.x())/qn);
		tfm.Set(1,2,-n.y()*q.x()/qn);
		tfm.Set(1,3,-n.z()*q.x()/qn);
		tfm.Set(1,4,q.x()*on/qn);

		tfm.Set(2,1,-n.x()*q.y()/qn);
		tfm.Set(2,2,1.0-n.y()*q.y()/qn);
		tfm.Set(2,3,-n.z()*q.y()/qn);
		tfm.Set(2,4,q.y()*on/qn);

		tfm.Set(3,1,-n.x()*q.z()/qn);
		tfm.Set(3,2,-n.y()*q.z()/qn);
		tfm.Set(3,3,1.0-n.z()*q.z()/qn);
		tfm.Set(3,4,q.z()*on/qn);

		tfm.Set(4,1,0.0);
		tfm.Set(4,2,0.0);
		tfm.Set(4,3,0.0);
		tfm.Set(4,4,1.0);

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsGetBisectorPlane(YsPlane &pln,const YsVec3 &p1,const YsVec3 &q1,const YsVec3 &p2,const YsVec3 &q2)
{
	auto v1=q1-p1;
	auto v2=q2-p2;
	if(YSOK!=v1.Normalize() || YSOK!=v2.Normalize())
	{
		return YSERR;
	}


	if(v1*v2<0.0)
	{
		v2=-v2;
	}
	auto v=v1+v2;
	if(YSOK!=v.Normalize())
	{
		return YSERR;
	}

	auto v3=p1-p2;
	if(YSOK!=v.Normalize())
	{
		// Special case: Two lines converges at p1.
		v3=v2-v1; // (p2+v2)-(p1+v1), but p1=p2.
		if(YSOK!=v.Normalize())
		{
			// Two lines are identical.
			return YSERR;
		}
	}

	auto t=v^v3;
	auto n=t^v;
	if(YSOK!=n.Normalize())
	{
		// Two lines are identical.
		//   v1 and v2 are parallel, and
		//   p2 lies on the extension or p1,v1 (or vise-versa)
		return YSERR;
	}

	YsVec3 c1,c2;
	if(YSOK==YsGetNearestPointOfTwoLine(c1,c2,p1,p1+v1,p2,p2+v2))
	{
		pln.Set((c1+c2)/2.0,n);
		return YSOK;
	}
	else // Two lines are parallel.
	{
		YsVec3 c1;
		if(YSOK==YsGetNearestPointOnLine3(c1,p1,p1+v1,p2))
		{
			pln.Set((c1+p2)/2.0,n);
			return YSOK;
		}
	}
	return YSERR;
}

YSINTERSECTION YsCheckTriangleIntersection(const YsVec2 t0[3],const YsVec2 t1[3])
{
	YSSIDE sideOfVtx[2][3];
	YSSIDE sideOfEdge[2][3];
	const YsVec2 *const t[2]=
	{
		t0,t1
	};
	int share[2][3]=
	{
		{YSFALSE,YSFALSE,YSFALSE},
		{YSFALSE,YSFALSE,YSFALSE},
	};


	for(int e0=0; e0<3; ++e0)
	{
		for(int e1=0; e1<3; ++e1)
		{
			if(t0[e0]==t1[e1])
			{
				share[0][e0]=YSTRUE;
				share[1][e1]=YSTRUE;
			}

			YsVec2 crs;
			if(YSOK==YsGetLineIntersection2(crs,t0[e0],t0[(e0+1)%3],t1[e1],t1[(e1+1)%3]) &&
			   YSTRUE==YsCheckInBetween2(crs,t0[e0],t0[(e0+1)%3]) &&
			   YSTRUE==YsCheckInBetween2(crs,t1[e1],t1[(e1+1)%3]) &&
			   crs!=t0[e0] &&
			   crs!=t0[(e0+1)%3] &&
			   crs!=t1[e1] &&
			   crs!=t1[(e1+1)%3])
			{
				return YSINTERSECT; // Clear intersection
			}
		}
	}


	auto nPointShare=share[0][0]+share[0][1]+share[0][2]+share[1][0]+share[1][1]+share[1][2];
	if(6==nPointShare)
	{
		return YSOVERLAP;  // Same triangle.  nPointShare can be 3, if one triangle is a point.
	}


	int nBoundary=0;
	for(int i=0; i<2; ++i)
	{
		for(int v=0; v<3; ++v)
		{
			sideOfVtx[i][v]=YsCheckInsideTriangle2(t[i][v],t[1-i]);
			if(YSINSIDE==sideOfVtx[i][v])
			{
				return YSINTERSECT;  
			}
			if(YSBOUNDARY==sideOfVtx[i][v])
			{
				++nBoundary;
			}
			sideOfEdge[i][v]=YsCheckInsideTriangle2((t[i][v]+t[i][(v+1)%3])/2.0,t[1-i]);
			if(YSINSIDE==sideOfEdge[i][v])
			{
				return YSINTERSECT;  
			}
			if(YSBOUNDARY==sideOfEdge[i][v])
			{
				++nBoundary;  
			}
		}
	}


	if(0<nBoundary || 0<nPointShare)
	{
		return YSSHAREPOINT;
	}


	return YSAPART;
}

YSINTERSECTION YsCheckTriangleIntersection(const YsVec3 t0[3],const YsVec3 t1[3],const YsVec3 &nom)
{
	YsMatrix3x3 toXY;
	toXY.MakeToXY(nom);

	YsVec2 s0[3],s1[3];
	s0[0]=(toXY*t0[0]).xy();
	s0[1]=(toXY*t0[1]).xy();
	s0[2]=(toXY*t0[2]).xy();
	s1[0]=(toXY*t1[0]).xy();
	s1[1]=(toXY*t1[1]).xy();
	s1[2]=(toXY*t1[2]).xy();

	return YsCheckTriangleIntersection(s0,s1);
}

void YsClipLineByPolygon(
    YsArray <YsVec2,2> &inside,YsArray <YsVec2,2> &outside,
    const YsVec2 p0,const YsVec2 p1,
    YsConstArrayMask <YsVec2> plg,
    YsRect2 plgBbx)
{
	inside.CleanUp();
	outside.CleanUp();
	if((p0.x()<plgBbx.Min().x() && p1.x()<plgBbx.Min().x()) ||
	   (p0.y()<plgBbx.Min().y() && p1.y()<plgBbx.Min().y()) ||
	   (p0.x()>plgBbx.Max().x() && p1.x()>plgBbx.Max().x()) ||
	   (p0.y()>plgBbx.Max().y() && p1.y()>plgBbx.Max().y()))
	{
		// Obviously outside.
		outside.Add(p0);
		outside.Add(p1);
		return;
	}

	YsArray <YsVec2,4> itsc; // In most case, 1 intersection.
	for(auto idx : plg.AllIndex())
	{
		auto &q0=plg[idx];
		auto &q1=plg.GetCyclic(idx+1);

		YsVec2 crs;
		if(YSOK==YsGetLineIntersection2(crs,p0,p1,q0,q1) &&
		   YSTRUE==YsCheckInBetween2(crs,p0,p1) &&
		   YSTRUE==YsCheckInBetween2(crs,q0,q1) &&
		   crs!=p0 &&
		   crs!=p1)
		{
			itsc.Add(crs);
		}
	}

	if(0==itsc.GetN())
	{
		auto side=YsCheckInsidePolygon2((p0+p1)/2.0,plg.GetN(),plg);
		if(YSINSIDE==side)
		{
			inside.Add(p0);
			inside.Add(p1);
		}
		else
		{
			outside.Add(p0);
			outside.Add(p1);
		}
	}
	else
	{
		if(1<itsc.GetN())
		{
			YsArray <double,4> dist;
			dist.Resize(itsc.GetN());
			for(auto idx : itsc.AllIndex())
			{
				dist[idx]=(itsc[idx]-p0)*(p1-p0);
			}
			YsSimpleMergeSort <double,YsVec2> (dist.GetN(),dist,itsc);
		}

		{
			auto side=YsCheckInsidePolygon2((p0+itsc[0])/2.0,plg.GetN(),plg);
			if(YSINSIDE==side)
			{
				inside.Add(p0);
				inside.Add(itsc[0]);
			}
			else
			{
				outside.Add(p0);
				outside.Add(itsc[0]);
			}
		}

		for(YSSIZE_T idx=0; idx<itsc.GetN()-1; ++idx)
		{
			if(itsc[idx]==itsc[idx+1])
			{
				continue;
			}

			auto side=YsCheckInsidePolygon2((itsc[idx]+itsc[idx+1])/2.0,plg.GetN(),plg);
			if(YSINSIDE==side)
			{
				inside.Add(itsc[idx]);
				inside.Add(itsc[idx+1]);
			}
			else
			{
				outside.Add(itsc[idx]);
				outside.Add(itsc[idx+1]);
			}
		}

		{
			auto side=YsCheckInsidePolygon2((itsc.Last()+p1)/2.0,plg.GetN(),plg);
			if(YSINSIDE==side)
			{
				inside.Add(itsc.Last());
				inside.Add(p1);
			}
			else
			{
				outside.Add(itsc.Last());
				outside.Add(p1);
			}
		}
	}
}

YSBOOL YsCheckOutsideBoundingBox(const YsVec3 bbx[2],YSSIZE_T np,const YsVec3 p[])
{
	bool allAbove=true,allBelow=true,allLeft=true,allRight=true,allBehind=true,allFront=true;
	for(YSSIZE_T i=0; i<np; ++i)
	{
		auto c=p[i];
		if(c.y()<bbx[1].y())
		{
			allAbove=false;
		}
		if(bbx[0].y()<c.y())
		{
			allBelow=false;
		}
		if(c.x()<bbx[1].x())
		{
			allRight=false;
		}
		if(bbx[0].x()<c.x())
		{
			allLeft=false;
		}
		if(c.z()<bbx[1].z())
		{
			allBehind=false;
		}
		if(bbx[0].z()<c.z())
		{
			allFront=false;
		}
	}
	if(true==allAbove || true==allBelow || true==allLeft || true==allRight || true==allBehind || true==allFront)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsCheckOutsideBoundingBox(const YsVec3 bbx[2],YsConstArrayMask <YsVec3> p)
{
	return YsCheckOutsideBoundingBox(bbx,p.size(),p.data());
}

YSBOOL YsCheckOutsideBoundingBox(const YsVec2 bbx[2],YSSIZE_T np,const YsVec2 p[])
{
	bool allAbove=true,allBelow=true,allLeft=true,allRight=true;
	for(YSSIZE_T i=0; i<np; ++i)
	{
		auto c=p[i];
		if(c.y()<bbx[1].y())
		{
			allAbove=false;
		}
		if(bbx[0].y()<c.y())
		{
			allBelow=false;
		}
		if(c.x()<bbx[1].x())
		{
			allRight=false;
		}
		if(bbx[0].x()<c.x())
		{
			allLeft=false;
		}
	}
	if(true==allAbove || true==allBelow || true==allLeft || true==allRight)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsCheckOutsideBoundingBox(const YsVec2 bbx[2],YsConstArrayMask <YsVec2> p)
{
	return YsCheckOutsideBoundingBox(bbx,p.size(),p.data());
}


////////////////////////////////////////////////////////////

template <class YsVec>
inline double YsTriangulationMaxAngleCos(const YsVec &p0,const YsVec &p1,const YsVec &p2)
{
	auto v0=p0-p1;
	auto v1=p2-p1;
	v0.Normalize();
	v1.Normalize();
	auto cost=v0*v1;

	v0=p1-p2;
	v1=p0-p2;
	v0.Normalize();
	v1.Normalize();
	YsMakeSmaller(cost,v0*v1);

	v0=p2-p0;
	v1=p1-p0;
	v0.Normalize();
	v1.Normalize();
	YsMakeSmaller(cost,v0*v1);

	return cost;
}

static YSSIZE_T NextVertexPtr(const YsConstArrayMask <YSSIZE_T> &vtxPtr,YSSIZE_T idx)
{
	auto nextIdx=idx+1;
	if(vtxPtr.size()<=nextIdx)
	{
		nextIdx=0;
	}
	while(0>vtxPtr.GetCyclic(nextIdx))
	{
		++nextIdx;
		if(vtxPtr.size()<=nextIdx)
		{
			nextIdx=0;
		}
	}
	return nextIdx;
}

static YSSIZE_T PrevVertexPtr(const YsConstArrayMask <YSSIZE_T> &vtxPtr,YSSIZE_T idx)
{
	auto prevIdx=idx-1;
	if(0>prevIdx)
	{
		prevIdx=vtxPtr.size()-1;
	}
	while(0>vtxPtr[prevIdx])
	{
		--prevIdx;
		if(0>prevIdx)
		{
			prevIdx=vtxPtr.size()-1;
		}
	}
	return prevIdx;
}

template <class YsVec>
YsArray <YSSIZE_T> YsTriangulateConvexPolygonTemplate(const YsConstArrayMask <YsVec> &plg)
{
	YsArray <YSSIZE_T> triIdx;

	YsAVLTree <double,YSSIZE_T> maxAngleQueue;
	YsArray <YSSIZE_T> vtxPtr;
	YsArray <YsAVLTree <double,YSSIZE_T>::NodeHandle> plgToTreeNode;

	for(YSSIZE_T idx=0; idx<plg.size(); ++idx)
	{
		vtxPtr.push_back(idx);

		auto pm=plg.GetCyclic(idx-2);
		auto p0=plg.GetCyclic(idx-1);
		auto p1=plg[idx];
		auto p2=plg.GetCyclic(idx+1);
		auto px=plg.GetCyclic(idx+2);

		auto cost=-YsTriangulationMaxAngleCos(p0,p1,p2);
		auto costm=-YsTriangulationMaxAngleCos(pm,p0,p2);
		auto costx=-YsTriangulationMaxAngleCos(p0,p2,px);
		auto ndHd=maxAngleQueue.Insert(YsGreatestOf(cost,costm,costx),idx);

		plgToTreeNode.push_back(ndHd);
	}

	while(3<=maxAngleQueue.size())
	{
		auto first=maxAngleQueue.First();
		auto idx=maxAngleQueue[first];

		auto prevIdx=PrevVertexPtr(vtxPtr,idx);
		auto nextIdx=NextVertexPtr(vtxPtr,idx);

		triIdx.push_back(prevIdx);
		triIdx.push_back(idx);
		triIdx.push_back(nextIdx);

		vtxPtr[idx]=-1;
		if(plgToTreeNode[idx]!=first)
		{
			fprintf(stderr,"Error!\n");
			fprintf(stderr,"Vtx to tree node table broken.\n");
		}
		maxAngleQueue.Delete(plgToTreeNode[idx]);
		plgToTreeNode[idx]=maxAngleQueue.Null();

		for(int i=0; i<2; ++i)
		{
			YSSIZE_T toUpdate=(0==i ? prevIdx : nextIdx);
			YSSIZE_T triVtx[5]=
			{
				-1,-1,toUpdate,-1,-1
			};
			triVtx[1]=PrevVertexPtr(vtxPtr,toUpdate);
			triVtx[3]=NextVertexPtr(vtxPtr,toUpdate);

			triVtx[0]=PrevVertexPtr(vtxPtr,triVtx[1]);
			triVtx[4]=NextVertexPtr(vtxPtr,triVtx[3]);

			maxAngleQueue.Delete(plgToTreeNode[toUpdate]);

			auto pm=plg[triVtx[0]];
			auto p0=plg[triVtx[1]];
			auto p1=plg[triVtx[2]];
			auto p2=plg[triVtx[3]];
			auto px=plg[triVtx[4]];
			auto cost=-YsTriangulationMaxAngleCos(p0,p1,p2);
			auto costm=-YsTriangulationMaxAngleCos(pm,p0,p2);
			auto costx=-YsTriangulationMaxAngleCos(p0,p2,px);
			auto ndHd=maxAngleQueue.Insert(YsGreatestOf(cost,costm,costx),toUpdate);

			plgToTreeNode[toUpdate]=ndHd;
		}
	}

	return triIdx;
}

YsArray <YSSIZE_T> YsTriangulateConvexPolygon(const YsConstArrayMask <YsVec2> &plg)
{
	return YsTriangulateConvexPolygonTemplate<YsVec2>(plg);
}

YsArray <YSSIZE_T> YsTriangulateConvexPolygon(const YsConstArrayMask <YsVec3> &plg)
{
	return YsTriangulateConvexPolygonTemplate<YsVec3>(plg);
}

YSBOOL YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec2> plg)
{
	for(int i=0; i<plg.size(); ++i)
	{
		const YsVec2 edge0[2]=
		{
			plg[i],
			plg.GetCyclic(i+1)
		};
		for(int j=0; j<plg.size(); ++j)
		{
			if((j+1)%plg.size()!=i &&
			   j!=i &&
			   (i+1)%plg.size()!=j)
			{
				const YsVec2 edge1[2]=
				{
					plg[j],
					plg.GetCyclic(j+1)
				};

				YsVec2 itsc;
				if(YSOK==YsGetLineIntersection2(itsc,edge0[0],edge0[1],edge1[0],edge1[1]))
				{
					if(YSTRUE==YsCheckInBetween2(itsc,edge0[0],edge0[1]) &&
					   YSTRUE==YsCheckInBetween2(itsc,edge1[0],edge1[1]))
					{
						return YSTRUE;
					}
				}
			}
		}
	}
	return YSFALSE;
}
YSBOOL YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> plg,const YsVec3 &nom)
{
	YsMatrix3x3 toXY;
	toXY.MakeToXY(nom);
	YsArray <YsVec2,8> plg2d;
	for(auto vtx : plg)
	{
		plg2d.push_back((toXY*vtx).xy());
	}
	return YsCheckPolygonSelfIntersection(plg2d);
}
YSBOOL YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> plg)
{
	auto nom=YsGetAverageNormalVector(plg);
	return YsCheckPolygonSelfIntersection(plg,nom);
}

YSBOOL YsCheckPolygonSelfIntersection(YSSIZE_T np,const YsVec2 p[])
{
	return YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec2> (np,p));
}
YSBOOL YsCheckPolygonSelfIntersection(YSSIZE_T np,const YsVec3 p[],const YsVec3 &nom)
{
	return YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> (np,p),nom);
}
YSBOOL YsCheckPolygonSelfIntersection(YSSIZE_T np,const YsVec3 p[])
{
	return YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> (np,p));
}
