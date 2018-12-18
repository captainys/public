/* ////////////////////////////////////////////////////////////

File Name: ysclip.cpp
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

#include "ysclip.h"

#include <string.h>
#include "yswizard.h"
#include "ysmalloc.h"
#include "ysadvgeometry.h"

YSRESULT YsClipLineSegByRightSide(YsVec2 &c1,YsVec2 &c2,double y);
YSRESULT YsClipLineSegByLeftSide(YsVec2 &c1,YsVec2 &c2,double y);
YSRESULT YsClipLineSegByBottomSide(YsVec2 &c1,YsVec2 &c2,double y);
YSRESULT YsClipLineSegByTopSide(YsVec2 &c1,YsVec2 &c2,double y);

YSRESULT YsClipLineSeg2(YsVec2 &c1,YsVec2 &c2,const YsVec2 &l1,const YsVec2 &l2,const YsVec2 &range1,const YsVec2 &range2)
{
	c1=l1;
	c2=l2;
	if(YsClipLineSegByTopSide(c1,c2,YsGreater(range1.y(),range2.y()))==YSOK &&
	   YsClipLineSegByBottomSide(c1,c2,YsSmaller(range1.y(),range2.y()))==YSOK &&
	   YsClipLineSegByRightSide(c1,c2,YsGreater(range1.x(),range2.x()))==YSOK &&
	   YsClipLineSegByLeftSide(c1,c2,YsSmaller(range1.x(),range2.x()))==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsClipLineSegByTopSide(YsVec2 &c1,YsVec2 &c2,double topy)
{
	double cut;
	// if(c1.y()>topy && c2.y()>topy)                           // Modified 2000/12/07
	if(c1.y()>topy+YsTolerance && c2.y()>topy+YsTolerance)      // Modified 2000/12/07
	{
		return YSERR;
	}
	else if(YsAbs(c1.y()-c2.y())<YsTolerance)
	{
		return YSOK;  // Note : One of them are below the top side and another is at the same height.
	}
	// else if(c1.y()>topy || c2.y()>topy)                           // Modified 2000/12/07
	else if(c1.y()>topy+YsTolerance || c2.y()>topy+YsTolerance)      // Modified 2000/12/07
	{
		cut=c1.x()+((c2.x()-c1.x())/(c2.y()-c1.y()))*(topy-c1.y());
		// if(c1.y()>topy)           // Modified 2000/12/07
		if(c1.y()>topy+YsTolerance)  // Modified 2000/12/07
		{
			c1.Set(cut,topy);
		}
		// else if(c2.y()>topy)      // Modified 2000/12/07
		else if(c2.y()>topy+YsTolerance)
		{
			c2.Set(cut,topy);
		}
		return YSOK;
	}
	return YSOK;
}

YSRESULT YsClipLineSegByBottomSide(YsVec2 &c1,YsVec2 &c2,double btmy)
{
	double cut;
	// if(c1.y()<btmy && c2.y()<btmy)                       // Modified 2000/12/07
	if(c1.y()<btmy-YsTolerance && c2.y()<btmy-YsTolerance)  // Modified 2000/12/07
	{
		return YSERR;
	}
	else if(YsAbs(c1.y()-c2.y())<YsTolerance)
	{
		return YSOK;  // Note : One of them are above the bottom side and another is at the same height.
	}
	// else if(c1.y()<btmy || c2.y()<btmy)                      // Modified 2000/12/07
	else if(c1.y()<btmy-YsTolerance || c2.y()<btmy-YsTolerance) // Modified 2000/12/07
	{
		cut=c1.x()+((c2.x()-c1.x())/(c2.y()-c1.y()))*(btmy-c1.y());
		// if(c1.y()<btmy)            // Modified 2000/12/07
		if(c1.y()<btmy-YsTolerance)   // Modified 2000/12/07
		{
			c1.Set(cut,btmy);
		}
		// else if(c2.y()<btmy)           // Modified 2000/12/07
		else if(c2.y()<btmy-YsTolerance)  // Modified 2000/12/07
		{
			c2.Set(cut,btmy);
		}
		return YSOK;
	}
	return YSOK;
}

YSRESULT YsClipLineSegByLeftSide(YsVec2 &c1,YsVec2 &c2,double lftx)
{
	double cut;
	// if(c1.x()<lftx && c2.x()<lftx)                     // Modified 2000/12/07
	if(c1.x()<lftx-YsTolerance && c2.x()<lftx-YsTolerance)// Modified 2000/12/07
	{
		return YSERR;
	}
	else if(YsAbs(c1.x()-c2.x())<YsTolerance)
	{
		return YSOK;
	}
	// else if(c1.x()<lftx || c2.x()<lftx)                     // Modified 2000/12/07
	else if(c1.x()<lftx-YsTolerance || c2.x()<lftx-YsTolerance)// Modified 2000/12/07
	{
		cut=c1.y()+((c2.y()-c1.y())/(c2.x()-c1.x()))*(lftx-c1.x());
		// if(c1.x()<lftx)         // Modified 2000/12/07
		if(c1.x()<lftx-YsTolerance)// Modified 2000/12/07
		{
			c1.Set(lftx,cut);
		}
		// if(c2.x()<lftx)         // Modified 2000/12/07
		if(c2.x()<lftx-YsTolerance)// Modified 2000/12/07
		{
			c2.Set(lftx,cut);
		}
		return YSOK;
	}
	return YSOK;
}

YSRESULT YsClipLineSegByRightSide(YsVec2 &c1,YsVec2 &c2,double ritx)
{
	double cut;
	// if(c1.x()>ritx && c2.x()>ritx)                      // Modified 2000/12/07
	if(c1.x()>ritx+YsTolerance && c2.x()>ritx+YsTolerance) // Modified 2000/12/07
	{
		return YSERR;
	}
	else if(YsAbs(c1.x()-c2.x())<YsTolerance)
	{
		return YSOK;
	}
	// else if(c1.x()>ritx || c2.x()>ritx)                      // Modified 2000/12/07
	else if(c1.x()>ritx+YsTolerance || c2.x()>ritx+YsTolerance) // Modified 2000/12/07
	{
		cut=c1.y()+((c2.y()-c1.y())/(c2.x()-c1.x()))*(ritx-c1.x());
		// if(c1.x()>ritx)          // Modified 2000/12/07
		if(c1.x()>ritx+YsTolerance) // Modified 2000/12/07
		{
			c1.Set(ritx,cut);
		}
		// if(c2.x()>ritx)          // Modified 2000/12/07
		if(c2.x()>ritx+YsTolerance) // Modified 2000/12/07
		{
			c2.Set(ritx,cut);
		}
		return YSOK;
	}
	return YSOK;
}


////////////////////////////////////////////////////////////

static YSRESULT YsClipInfinite2DLineByX(YsVec2 &itsc,const YsVec2 &org,const YsVec2 &vec,const double &x)
{
	if(YsAbs(vec.x())>YsTolerance)
	{
		double y,slope;
		slope=vec.y()/vec.x();
		y=org.y()+slope*(x-org.x());
		itsc.Set(x,y);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

static YSRESULT YsClipInfinite2DLineByY(YsVec2 &itsc,const YsVec2 &org,const YsVec2 &vec,const double &y)
{
	if(YsAbs(vec.y())>YsTolerance)
	{
		double x,slope;
		slope=vec.x()/vec.y();
		x=org.x()+slope*(y-org.y());
		itsc.Set(x,y);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

static YSRESULT YsClipInfiniteLine2_FindFirstIntersection
    (YsVec2 &itsc,const YsVec2 &org,const YsVec2 &vec,const YsVec2 &range1,const YsVec2 &range2)
{
	// must have range1<range2
	if((YsClipInfinite2DLineByX(itsc,org,vec,range1.x())==YSOK &&
	    range1.y()-YsTolerance<itsc.y() && itsc.y()<range2.y()+YsTolerance) ||
	   (YsClipInfinite2DLineByX(itsc,org,vec,range2.x())==YSOK &&
	    range1.y()-YsTolerance<itsc.y() && itsc.y()<range2.y()+YsTolerance))
	{
		itsc.SetY(YsBound(itsc.y(),range1.y(),range2.y()));
		return YSOK;
	}
	else if((YsClipInfinite2DLineByY(itsc,org,vec,range1.y())==YSOK &&
	         range1.x()-YsTolerance<itsc.x() && itsc.x()<range2.x()+YsTolerance) ||
	        (YsClipInfinite2DLineByY(itsc,org,vec,range2.y())==YSOK &&
	         range1.x()-YsTolerance<itsc.x() && itsc.x()<range2.x()+YsTolerance))
	{
		itsc.SetX(YsBound(itsc.x(),range1.x(),range2.x()));
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

static YSRESULT YsClipInfiniteLine2_FindSecondIntersection
    (YsVec2 &itsc,const YsVec2 &firstItsc,const YsVec2 &vec,const YsVec2 &range1,const YsVec2 &range2)
{
	// must have range1<range2
	YSRESULT found;
	double dist,d;
	YsVec2 candidate;
	YsVec2 const *range[2];
	int i;

	dist=-1.0;  // 0.0 should be OK
	found=YSERR;

	range[0]=&range1;
	range[1]=&range2;

	for(i=0; i<2; i++)
	{
		if(YsClipInfinite2DLineByX(candidate,firstItsc,vec,range[i]->x())==YSOK &&
		   range1.y()-YsTolerance<candidate.y() && candidate.y()<range2.y()+YsTolerance)
		{
			candidate.SetY(YsBound(candidate.y(),range1.y(),range2.y()));
			d=(candidate-firstItsc).GetSquareLength();
			if(d>=dist)
			{
				itsc=candidate;
				dist=d;
				found=YSOK;
			}
		}

		if(YsClipInfinite2DLineByY(candidate,firstItsc,vec,range[i]->y())==YSOK &&
		   range1.x()-YsTolerance<candidate.x() && candidate.x()<range2.x()+YsTolerance)
		{
			candidate.SetX(YsBound(candidate.x(),range1.x(),range2.x()));
			d=(candidate-firstItsc).GetSquareLength();
			if(d>=dist)
			{
				itsc=candidate;
				dist=d;
				found=YSOK;
			}
		}
	}

	return found;
}

YSRESULT YsClipInfiniteLine2
    (YsVec2 &c1,YsVec2 &c2,
     const YsVec2 &org,const YsVec2 &vec,const YsVec2 &range1,const YsVec2 &range2)
{
	YsVec2 t1,t2,nv,r1,r2;
	YsBoundingBoxMaker2 bbx;
	nv=vec;
	nv.Normalize();

	bbx.Begin(range1);
	bbx.Add(range2);
	bbx.Get(r1,r2);

	if(YsClipInfiniteLine2_FindFirstIntersection(t1,org,nv,r1,r2)==YSOK &&
	   YsClipInfiniteLine2_FindSecondIntersection(t2,t1,nv,r1,r2)==YSOK)
	{
		if((t2-t1)*vec>=0.0)
		{
			c1=t1;
			c2=t2;
		}
		else
		{
			c1=t2;
			c2=t1;
		}
		return YSOK;
	}
	return YSERR;
}


////////////////////////////////////////////////////////////

static YSRESULT YsClip3DLine
    (YsVec3 &clip,const YsVec3 &org,const YsVec3 &vec,const double &clipPoint,int component)
{
	int c1,c2;
	double buf[3],slope[3];

	if(component==0)
	{
		c1=1;
		c2=2;
	}
	else if(component==1)
	{
		c1=0;
		c2=2;
	}
	else
	{
		c1=0;
		c2=1;
	}

	if(YsAbs(vec.GetValue()[component])>=YsTolerance)
	{
		slope[c1]=vec.GetValue()[c1]/vec.GetValue()[component];
		slope[c2]=vec.GetValue()[c2]/vec.GetValue()[component];

		buf[component]=clipPoint;
		buf[c1]=org.GetValue()[c1]+slope[c1]*(clipPoint-org.GetValue()[component]);
		buf[c2]=org.GetValue()[c2]+slope[c2]*(clipPoint-org.GetValue()[component]);

		clip.Set(buf[0],buf[1],buf[2]);

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

static YSRESULT YsClipInfiniteLine3_FindFirstIntersection
    (YsVec3 &itsc,const YsVec3 &org,const YsVec3 &vec,const YsVec3 range[2])
{
	int i,j,k;

	for(i=0; i<3; i++)
	{
		for(j=0; j<2; j++)
		{
			if(YsClip3DLine(itsc,org,vec,range[j].GetValue()[i],i)==YSOK)
			{
				for(k=0; k<3; k++)
				{
					if(i!=k &&
					   (itsc.GetValue()[k]<range[0].GetValue()[k]-YsTolerance ||
					    range[1].GetValue()[k]+YsTolerance<itsc.GetValue()[k]))
					{
						goto NEXTPLANE;
					}
				}
				return YSOK;
			}
		NEXTPLANE:
			;
		}
	}
	return YSERR;
}


static YSRESULT YsClipInfiniteLine3_FindSecondIntersection
    (YsVec3 &itsc,const YsVec3 &first,const YsVec3 &vec,const YsVec3 range[2])
{
	int i,j,k;
	double dist,d;
	YSRESULT found;
	YsVec3 candidate;

	found=YSERR;
	dist=-1.0;
	candidate.Set(0.0,0.0,0.0);
	for(i=0; i<3; i++)
	{
		for(j=0; j<2; j++)
		{
			if(YsClip3DLine(candidate,first,vec,range[j].GetValue()[i],i)==YSOK)
			{
				for(k=0; k<3; k++)
				{
					if(i!=k &&
					   (candidate.GetValue()[k]<range[0].GetValue()[k]-YsTolerance ||
					    range[1].GetValue()[k]+YsTolerance<candidate.GetValue()[k]))
					{
						goto NEXTPLANE;
					}
				}
				found=YSOK;
				d=(first-candidate).GetSquareLength();
				if(d>dist)
				{
					itsc=candidate;
					dist=d;
				}
			}
		NEXTPLANE:
			;
		}
	}
	return found;
}


YSRESULT YsClipInfiniteLine3
    (YsVec3 &c1,YsVec3 &c2,
     const YsVec3 &org,const YsVec3 &vec,const YsVec3 &range1,const YsVec3 &range2)
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 r[2];
	YsVec3 t1,t2;

	bbx.Begin(range1);
	bbx.Add(range2);
	bbx.Get(r[0],r[1]);

	if(YsClipInfiniteLine3_FindFirstIntersection(t1,org,vec,r)==YSOK &&
	   YsClipInfiniteLine3_FindSecondIntersection(t2,t1,vec,r)==YSOK)
	{
		if((t2-t1)*vec>=0.0)
		{
			c1=t1;
			c2=t2;
		}
		else
		{
			c1=t2;
			c2=t1;
		}
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

static YSRESULT YsClipPolygon3_ByOnePlane
   (int &newNp,YsVec3 newp[],int bufLen,
    int np,const YsVec3 p[],const YsVec3 &threshold,int component,YSBOOL removeGreater)
{
	const double *v1,*v2,*thr;
	double v[3];
	YSBOOL in;

	thr=threshold.GetValue();

	if(bufLen>0)
	{
		newNp=0;
		v1=p[0].GetValue();
		if(removeGreater==YSTRUE)
		{
			if(v1[component]<=thr[component])
			{
				in=YSTRUE;
				newp[newNp]=p[0];
				newNp++;
			}
			else
			{
				in=YSFALSE;
			}
		}
		else
		{
			if(v1[component]>=thr[component])
			{
				in=YSTRUE;
				newp[newNp]=p[0];
				newNp++;
			}
			else
			{
				in=YSFALSE;
			}
		}

		int i;
		YSBOOL prevIn;
		for(i=0; i<np; i++)
		{
			v1=p[i].GetValue();
			v2=p[(i+1)%np].GetValue();

			prevIn=in;
			if(removeGreater==YSTRUE)
			{
				if(v2[component]<=thr[component])
				{
					in=YSTRUE;
				}
				else
				{
					in=YSFALSE;
				}
			}
			else
			{
				if(v2[component]>=thr[component])
				{
					in=YSTRUE;
				}
				else
				{
					in=YSFALSE;
				}
			}

			if(prevIn!=in && YsEqual(v1[component],v2[component])!=YSTRUE)
			{
				if(newNp<bufLen)
				{
					int e;
					for(e=0; e<3; e++)
					{
						if(e==component)
						{
							v[e]=thr[component];
						}
						else
						{
							double x1,y1,x2,y2,x,y;
							x1=v1[component];
							y1=v1[e];
							x2=v2[component];
							y2=v2[e];
							x=thr[component];
							y=(y2-y1)/(x2-x1)*(x-x1)+y1;
							v[e]=y;
						}
					}
					newp[newNp].Set(v[0],v[1],v[2]);
					newNp++;
				}
				else
				{
					return YSERR;
				}
			}

			if(in==YSTRUE && i+1<np) // i+1<np: p[0] is already checked before the loop.
			{
				if(newNp<bufLen)
				{
					newp[newNp].Set(v2[0],v2[1],v2[2]);
					newNp++;
				}
				else
				{
					return YSERR;
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsClipPolygon3(int &newNp,YsVec3 newp[],int bufLen,int np,const YsVec3 p[],const YsVec3 &range1,const YsVec3 &range2)
{
	YsVec3 min,max;
	YsBoundingBoxMaker3 bbx;
	int i,j,nIn,nOut[6];

	bbx.Begin(range1);
	bbx.Add(range2);
	bbx.Get(min,max);

	nIn=0;
	nOut[0]=0;
	nOut[1]=0;
	nOut[2]=0;
	nOut[3]=0;
	nOut[4]=0;
	nOut[5]=0;
	for(i=0; i<np; i++)
	{
		if(min.x()<=p[i].x() && p[i].x()<=max.x() &&
		   min.y()<=p[i].y() && p[i].y()<=max.y() &&
		   min.z()<=p[i].z() && p[i].z()<=max.z())
		{
			nIn++;
		}
		else
		{
			for(j=0; j<3; j++)
			{
				if(p[i].GetValue()[j]<min.GetValue()[j])
				{
					nOut[j*2  ]++;
				}
				else if(p[i].GetValue()[j]>max.GetValue()[j])
				{
					nOut[j*2+1]++;
				}
			}
		}
	}

	if(nIn==np)
	{
		if(bufLen>=np)
		{
			newNp=np;
			for(i=0; i<np; i++)
			{
				newp[i]=p[i];
			}
			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	if(nOut[0]==np || nOut[1]==np || nOut[2]==np ||
	   nOut[3]==np || nOut[4]==np || nOut[5]==np)
	{
		newNp=0;
		return YSOK;
	}

	YsOneMemoryBlock <YsVec3,1024> memblk1;
	YsOneMemoryBlock <YsVec3,1024> memblk2;
	YsOneMemoryBlock <YsVec3,1024> memblk3;
	YsOneMemoryBlock <YsVec3,1024> memblk4;
	YsOneMemoryBlock <YsVec3,1024> memblk5;
	YsVec3 *plg1,*plg2,*plg3,*plg4,*plg5;
	int nplg1,nplg2,nplg3,nplg4,nplg5;

	plg1=memblk1.GetMemoryBlock(bufLen);
	plg2=memblk2.GetMemoryBlock(bufLen);
	plg3=memblk3.GetMemoryBlock(bufLen);
	plg4=memblk4.GetMemoryBlock(bufLen);
	plg5=memblk5.GetMemoryBlock(bufLen);

	if(YsClipPolygon3_ByOnePlane(nplg1,plg1,bufLen,np   ,p   ,min,0,YSFALSE)==YSOK &&
	   YsClipPolygon3_ByOnePlane(nplg2,plg2,bufLen,nplg1,plg1,min,1,YSFALSE)==YSOK &&
	   YsClipPolygon3_ByOnePlane(nplg3,plg3,bufLen,nplg2,plg2,min,2,YSFALSE)==YSOK &&
	   YsClipPolygon3_ByOnePlane(nplg4,plg4,bufLen,nplg3,plg3,max,0,YSTRUE)==YSOK &&
	   YsClipPolygon3_ByOnePlane(nplg5,plg5,bufLen,nplg4,plg4,max,1,YSTRUE)==YSOK &&
	   YsClipPolygon3_ByOnePlane(newNp,newp,bufLen,nplg5,plg5,max,2,YSTRUE)==YSOK)
	{
		return YSOK;
	}
	return YSERR; // Buffer overflow
}

YSRESULT YsClipPolygonByPlane(int &newNp,YsVec3 newp[],int bufLen,int np,const YsVec3 p[],const YsPlane &pln,int whichSideToLive)
{
	newNp=0;

	YSSIZE_T toBegin=-1;

	YsArray <int,8> side(np,NULL);
	for(YSSIZE_T i=0; i<np; ++i)
	{
		side[i]=pln.GetSideOfPlane(p[i]);
		if(0>toBegin && side[i]==whichSideToLive)
		{
			toBegin=i;
		}
	}

	if(0>toBegin)
	{
		return YSERR;
	}

	int prevSide=whichSideToLive;
	for(YSSIZE_T q=0; q<np && newNp<bufLen; ++q)
	{
		auto idx=(toBegin+q)%np;
		if(side[idx]==whichSideToLive)
		{
			newp[newNp++]=p[idx];
			prevSide=whichSideToLive;

			if(newNp<bufLen && side.GetCyclic(idx+1)==-whichSideToLive)
			{
				YsVec3 itsc;
				pln.GetIntersection(itsc,p[idx],p[(idx+1)%np]-p[idx]);
				newp[newNp++]=itsc;
			}
		}
		else if(side[idx]==-whichSideToLive)
		{
			if(newNp<bufLen && side.GetCyclic(idx+1)==whichSideToLive)
			{
				YsVec3 itsc;
				pln.GetIntersection(itsc,p[idx],p[(idx+1)%np]-p[idx]);
				newp[newNp++]=itsc;
			}
			prevSide=side[idx];
		}
		else if(0==side[idx])
		{
			if(prevSide==whichSideToLive || side.GetCyclic(idx+1)==whichSideToLive)
			{
				newp[newNp++]=p[idx];
			}
			// Don't update prevSide if 0==side[idx].
		}
	}

	if(3<=newNp)
	{
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YSRESULT YsClipLineSeg3
   (YsVec3 &newP1,YsVec3 &newP2,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &range1,const YsVec3 &range2)
{
	int j,nInComp;
	YsVec3 min,max;
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(range1);
	bbx.Add(range2);
	bbx.Get(min,max);

	nInComp=0;
	for(j=0; j<3; j++)
	{
		if(min.GetValue()[j]<=p1.GetValue()[j] && p1.GetValue()[j]<=max.GetValue()[j] &&
		   min.GetValue()[j]<=p2.GetValue()[j] && p2.GetValue()[j]<=max.GetValue()[j])
		{
			nInComp++;
		}

		if(p1.GetValue()[j]<min.GetValue()[j] &&
		   p2.GetValue()[j]<min.GetValue()[j])
		{
			return YSERR;
		}
		if(p1.GetValue()[j]>max.GetValue()[j] &&
		   p2.GetValue()[j]>max.GetValue()[j])
		{
			return YSERR;
		}
	}

	if(nInComp==3)
	{
		newP1=p1;
		newP2=p2;
		return YSOK;
	}

	YsVec3 c1,c2;
	if(YsClipInfiniteLine3(c1,c2,p1,p2-p1,min,max)==YSOK)
	{
		if(YsCheckInBetween3(c1,p1,p2)==YSTRUE &&
		   YsCheckInBetween3(c2,p1,p2)==YSTRUE)
		{
			newP1=c1;
			newP2=c2;
			return YSOK;
		}
		else if(YsCheckInBetween3(c1,p1,p2)==YSTRUE)
		{
			// p1-c1 or c1-p2
			if(YsCheckInBetween3(p1,c1,c2)==YSTRUE)
			{
				newP1=p1;
				newP2=c1;
			}
			else
			{
				newP1=c1;
				newP2=p2;
			}
			return YSOK;
		}
		else if(YsCheckInBetween3(c2,p1,p2)==YSTRUE)
		{
			// p1-c2 or c2-p2
			if(YsCheckInBetween3(p1,c1,c2)==YSTRUE)
			{
				newP1=p1;
				newP2=c2;
			}
			else
			{
				newP1=c2;
				newP2=p2;
			}
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

YSRESULT YsZClip(YsVec3 &clipPos,const YsVec3 &p1,const YsVec3 &p2,const double &clipz)
{
	const double denom=(p2.z()-p1.z());
	if(YsTolerance<=fabs(denom))
	{
		const YsVec3 v=p2-p1;
		const double t=(clipz-p1.z())/denom;
		clipPos.Set(p1[0]+t*v[0],p1[1]+t*v[1],clipz);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsClipLineWithNearZ(
	YsVec3 &newP1,YsVec3 &newP2,const YsVec3 &p1,const YsVec3 &p2,const double &nearz,YSBOOL positivez)
{
	class TestIsFrontSide
	{
	public:
		inline YSBOOL IsFrontSide(const double tstz,const double nearz,YSBOOL positivez)
		{
			if(YSTRUE==positivez && nearz<=tstz)
			{
				return YSTRUE;
			}
			else if(YSTRUE!=positivez && tstz<=nearz)
			{
				return YSTRUE;
			}
			return YSFALSE;
		}
	};

	TestIsFrontSide tester;

	if(YSTRUE==tester.IsFrontSide(p1.z(),nearz,positivez) &&
	   YSTRUE==tester.IsFrontSide(p2.z(),nearz,positivez))
	{
		newP1=p1;
		newP2=p2;
		return YSOK;
	}
	else if(YSTRUE==tester.IsFrontSide(p1.z(),nearz,positivez) ||
	        YSTRUE==tester.IsFrontSide(p2.z(),nearz,positivez))
	{
		YsVec3 clipPos;
		if(YSOK==YsZClip(clipPos,p1,p2,nearz))
		{
			if(YSTRUE==tester.IsFrontSide(p2.z(),nearz,positivez))
			{
				newP1=clipPos;
				newP2=p2;
				return YSOK;
			}
			else
			{
				newP1=p1;
				newP2=clipPos;
				return YSOK;
			}
		}
	}
	return YSERR;
}
