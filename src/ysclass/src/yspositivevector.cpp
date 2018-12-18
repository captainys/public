/* ////////////////////////////////////////////////////////////

File Name: yspositivevector.cpp
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
#include "yspositivevector.h"
#include "ysadvgeometry.h"


YsVolumeSlasher::YsVolumeSlasher()
{
	shl.AttachSearchTable(&search);
}
YsVolumeSlasher::~YsVolumeSlasher()
{
}

void YsVolumeSlasher::CalculateVertexOnEdge(
    YsArray <YsShellVertexHandle,16> &vtHdInsert,
    YsArray <YsShell::VertexHandle,16> &vtHdOnPlane, // Not inserted, but already on plane.
    const YsPlane &pln)
{
	YsShellEdgeEnumHandle edHd;
	if(YSOK==search.RewindEdgeEnumHandle(shl,edHd))
	{
		for(;;)
		{
			YsShellVertexHandle edVtHd[2];
			search.GetEdge(shl,edVtHd[0],edVtHd[1],edHd);

			YsVec3 edVtPos[2];
			shl.GetVertexPosition(edVtPos[0],edVtHd[0]);
			shl.GetVertexPosition(edVtPos[1],edVtHd[1]);

			int sideOfPln[2];
			sideOfPln[0]=pln.GetSideOfPlane(edVtPos[0]);
			sideOfPln[1]=pln.GetSideOfPlane(edVtPos[1]);

			if(0>sideOfPln[0]*sideOfPln[1])
			{
				YsVec3 crs;
				if(YSOK==pln.GetIntersectionHighPrecision(crs,edVtPos[0],edVtPos[1]-edVtPos[0]))
				{
					YsShellVertexHandle crsVtHd=shl.AddVertexH(crs);
					vtHdInsert.Append(2,edVtHd);
					vtHdInsert.Append(crsVtHd);
				}
			}
			else if(0==sideOfPln[0] && 0<sideOfPln[1])
			{
				YsVec3 crs;
				if(YSOK==pln.GetIntersectionHighPrecision(crs,edVtPos[0],edVtPos[1]-edVtPos[0]))
				{
					shl.ModifyVertexPosition(edVtHd[0],crs);
					vtHdOnPlane.push_back(edVtHd[0]);
				}
			}
			else if(0==sideOfPln[1] && 0<sideOfPln[0])
			{
				YsVec3 crs;
				if(YSOK==pln.GetIntersectionHighPrecision(crs,edVtPos[1],edVtPos[0]-edVtPos[1]))
				{
					shl.ModifyVertexPosition(edVtHd[1],crs);
					vtHdOnPlane.push_back(edVtHd[1]);
				}
			}

			if(YSOK!=search.FindNextEdge(shl,edHd))
			{
				break;
			}
		}
	}
}

void YsVolumeSlasher::InsertVertexOnEdge(const YsConstArrayMask <YsShellVertexHandle> &vtHdInsert)
{
	int i;
	for(i=0; i<=vtHdInsert.GetN()-3; i+=3)
	{
		int nEdPl;
		const YsShellPolygonHandle *edPlHdIn;
		if(YSOK==search.FindPolygonListByEdge(nEdPl,edPlHdIn,shl,vtHdInsert[i],vtHdInsert[i+1]))
		{
			YsArray <YsShellPolygonHandle,2> edPlHd(nEdPl,edPlHdIn);

			int j;
			for(j=0; j<edPlHd.GetN(); j++)
			{
				YsArray <YsShellVertexHandle,16> plVtHd;
				shl.GetVertexListOfPolygon(plVtHd,edPlHd[j]);

				if(3<=plVtHd.GetN())
				{
					plVtHd.Append(plVtHd[0]);

					int k;
					YSBOOL mod=YSFALSE;
					for(k=0; k<plVtHd.GetN()-1; k++)
					{
						if(YSTRUE==YsSameEdge(plVtHd[k],plVtHd[k+1],vtHdInsert[i],vtHdInsert[i+1]))
						{
							plVtHd.Insert(k+1,vtHdInsert[i+2]);
							mod=YSTRUE;
							break;
						}
					}

					if(YSTRUE==mod)
					{
						plVtHd.DeleteLast();
						shl.ModifyPolygon(edPlHd[j],plVtHd.GetN(),plVtHd);
					}
				}
			}
		}
	}
}

void YsVolumeSlasher::SlashPolygon(const YsPlane &pln)
{
	YsShellPolygonHandle plHd=NULL;
	while(NULL!=(plHd=shl.FindNextPolygon(plHd)))
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsArray <YsShellVertexHandle,16> newPlVtHd;

		int nOnPlane=0,positive=-1,negative=-1;
		int onPlane[2];
		shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

		int i;
		for(i=0; i<nPlVt; i++)
		{
			YsVec3 vtPos;
			shl.GetVertexPosition(vtPos,plVtHd[i]);

			const int side=pln.GetSideOfPlane(vtPos);
			if(0==side)
			{
				if(2>nOnPlane)
				{
					onPlane[nOnPlane]=i;
				}
				nOnPlane++;
			}
			else if(0>side)
			{
				negative=i;
			}
			else // if(0<side)
			{
				positive=i;
			}
		}

		if(0<=negative && 0<=positive && 2==nOnPlane)
		{
			newPlVtHd.Clear();
			if(onPlane[0]<positive && positive<onPlane[1])
			{
				int i;
				for(i=onPlane[0]; i<=onPlane[1]; i++)
				{
					newPlVtHd.Append(plVtHd[i]);
				}
			}
			else
			{
				int i;
				for(i=onPlane[1]; i<nPlVt; i++)
				{
					newPlVtHd.Append(plVtHd[i]);
				}
				for(i=0; i<=onPlane[0]; i++)
				{
					newPlVtHd.Append(plVtHd[i]);
				}
			}

			if(3<=newPlVtHd.GetN())
			{
				shl.ModifyPolygon(plHd,newPlVtHd.GetN(),newPlVtHd);
			}
		}
	}
}

void YsVolumeSlasher::DeleteNegativePolygon(const YsPlane &pln)
{
	YsArray <YsShellPolygonHandle> plHdToDel;
	YsShellPolygonHandle plHd=NULL;
	while(NULL!=(plHd=shl.FindNextPolygon(plHd)))
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

		int i;
		for(i=0; i<nPlVt; i++)
		{
			YsVec3 vtPos;
			shl.GetVertexPosition(vtPos,plVtHd[i]);
			if(pln.GetSideOfPlane(vtPos)<0)
			{
				plHdToDel.Append(plHd);
				break;
			}
		}
	}

	YsArray <YsShellVertexHandle> vtHdToDel;
	YsShellVertexHandle vtHd=NULL;
	while(NULL!=(vtHd=shl.FindNextVertex(vtHd)))
	{
		YsVec3 vtPos;
		shl.GetVertexPosition(vtPos,vtHd);
		if(pln.GetSideOfPlane(vtPos)<0)
		{
			vtHdToDel.Append(vtHd);
		}
	}

	int i;
	for(i=0; i<plHdToDel.GetN(); i++)
	{
		shl.DeletePolygon(plHdToDel[i]);
	}
	for(i=0; i<vtHdToDel.GetN(); i++)
	{
		shl.DeleteVertex(vtHdToDel[i]);
	}
}

void YsVolumeSlasher::CloseHole(
    const YsPlane &pln,
    const YsConstArrayMask <YsShellVertexHandle> &vtHdInsert,
    const YsConstArrayMask <YsShellVertexHandle> &vtHdOnPlane)
{
	// All vtHdInsert[i*3+2] must be used.
	// Some or all of vtHdOnPlane may not be used.

	YsShell::Edge edge0(nullptr,nullptr);
	for(YSSIZE_T idx=0; idx<=vtHdInsert.size()-3 && nullptr==edge0[0]; idx+=3)
	{
		edge0=FindOpenEdgeFrom(vtHdInsert[idx+2]);
	}
	for(YSSIZE_T idx=0; idx<vtHdOnPlane.size() && nullptr==edge0[0]; ++idx)
	{
		edge0=FindOpenEdgeFrom(vtHdOnPlane[idx]);
	}

	if(nullptr!=edge0[0])
	{
		YsArray <YsShell::VertexHandle,16> plVtHd;
		plVtHd.push_back(edge0[0]);
		plVtHd.push_back(edge0[1]);
		for(;;) // Until it can no longer find the next.
		{
			YsArray <YsShell::VertexHandle,8> connVtHd;
			search.GetConnectedVertexList(connVtHd,shl,plVtHd.back());
			for(auto idx : connVtHd.ReverseIndex())
			{
				if(1!=search.GetNumPolygonUsingEdge(shl,plVtHd.back(),connVtHd[idx]) ||
				   connVtHd[idx]==plVtHd[plVtHd.size()-2])
				{
					connVtHd.DeleteBySwapping(idx);
				}
			}

			if(1!=connVtHd.size())
			{
				// Broken.
				return;
			}
			else
			{
				if(connVtHd[0]==plVtHd[0])
				{
					break;
				}
				plVtHd.push_back(connVtHd[0]);
			}
		}

		if(3<=plVtHd.size())
		{
			auto plHd=shl.AddPolygonH(plVtHd.size(),plVtHd.data());
			shl.SetNormalOfPolygon(plHd,-pln.GetNormal());
		}
	}
}

YsShell::Edge YsVolumeSlasher::FindOpenEdgeFrom(YsShell::VertexHandle vtHd0) const
{
	YsShell::Edge edge0(nullptr,nullptr);
	YsArray <YsShell::VertexHandle,8> allConnVtHd;
	search.GetConnectedVertexList(allConnVtHd,shl,vtHd0);
	for(auto connVtHd : allConnVtHd)
	{
		auto edPlHdPtr=search.FindPolygonListByEdge(shl,vtHd0,connVtHd);
		if(nullptr!=edPlHdPtr && 1==edPlHdPtr->size())
		{
			edge0[0]=vtHd0;
			edge0[1]=connVtHd;

			// The following lines will correctly orient polygons,
			// but may not be necessary. >>
			auto plVtHd=shl.GetPolygonVertex(edPlHdPtr->front());
			for(auto idx : plVtHd.AllIndex())
			{
				if(plVtHd[idx]==edge0[0] && plVtHd.GetCyclic(idx+1)==edge0[1])
				{
					std::swap(edge0[0],edge0[1]);
					break;
				}
				else if(plVtHd[idx]==edge0[1] && plVtHd.GetCyclic(idx+1)==edge0[0])
				{
					break;
				}
			}
			// <<

			return edge0;
		}
	}
	return edge0;
}

YSRESULT YsVolumeSlasher::SaveSrf(const char fn[]) const
{
	return shl.SaveSrf(fn);
}

////////////////////////////////////////////////////////////

YsPositiveVectorCalculator::YsPositiveVectorCalculator()
{
	Initialize();
}

YsPositiveVectorCalculator::~YsPositiveVectorCalculator()
{
}

void YsPositiveVectorCalculator::Initialize(void)
{
	YsShellVertexHandle vtHd[6];
	vtHd[0]=shl.AddVertexH(YsVec3( 1.0, 0.0, 0.0));
	vtHd[1]=shl.AddVertexH(YsVec3(-1.0, 0.0, 0.0));
	vtHd[2]=shl.AddVertexH(YsVec3( 0.0, 1.0, 0.0));
	vtHd[3]=shl.AddVertexH(YsVec3( 0.0,-1.0, 0.0));
	vtHd[4]=shl.AddVertexH(YsVec3( 0.0, 0.0, 1.0));
	vtHd[5]=shl.AddVertexH(YsVec3( 0.0, 0.0,-1.0));

	const int idx[]=
	{
		0,2,4,
		1,2,4,
		0,3,4,
		1,3,4,
		0,2,5,
		1,2,5,
		0,3,5,
		1,3,5
	};

	int i;
	YsShellVertexHandle triVtHd[3];
	for(i=0; i<8; i++)
	{
		triVtHd[0]=vtHd[idx[i*3]];
		triVtHd[1]=vtHd[idx[i*3+1]];
		triVtHd[2]=vtHd[idx[i*3+2]];
		shl.AddPolygonH(3,triVtHd);
	}

	nVec=0;
}

void YsPositiveVectorCalculator::AddVector(YsVec3 n)
{
	if(YSOK!=n.Normalize())
	{
		return;
	}

	if(nVec<3)
	{
		vec[nVec]=n;
	}
	nVec++;


	YsPlane pln(YsVec3(0.0,0.0,0.0),n);

	YsArray <YsShellVertexHandle,16> vtHdInsert,vtHdOnPlane;
	CalculateVertexOnEdge(vtHdInsert,vtHdOnPlane,pln);
	InsertVertexOnEdge(vtHdInsert);

	SlashPolygon(pln);
	DeleteNegativePolygon(pln);
}

YSRESULT YsPositiveVectorCalculator::GetPositiveVector(YsVec3 &vec) const
{
	if(0<shl.GetNumPolygon())
	{
		vec=YsVec3(0.0,0.0,0.0);
		YsShellPolygonHandle plHd=NULL;
		while(NULL!=(plHd=shl.FindNextPolygon(plHd)))
		{
			YsVec3 cen;
			shl.GetCenterOfPolygon(cen,plHd);
			vec+=cen;
		}
		return vec.Normalize();
	}
	return YSERR;
}

// See also positive-vector-2.docx
/* static */ YSRESULT YsPositiveVectorCalculator::Optimize(YsVec3 &vecOut,const YsVec3 &vecIn,const YSSIZE_T nRefVec,const YsVec3 refVec[],int nConst,const YsVec3 constVec[])
{
	YSRESULT improvement=YSERR;
	bool watch=false;

	YsArray <YSBOOL,16> isMinimum(nRefVec,NULL);
	YsArray <double,16> slope(nRefVec,NULL);
	YsArray <double,16> dotProd(nRefVec,NULL);

if(YsVec3(-0.921480,  -0.370117,   0.117849)==vecIn)
{
watch=true;
}

	vecOut=vecIn;
	if(true==watch)
	{
		YsPrintf("Watch! %s\n",__FUNCTION__);
		YsPrintf("vecIn=%s\n",vecIn.Txt());
	}

	ApplyConstraint(vecOut,nConst,constVec);

	double minDot=0.0;
	for(int i=0; i<nRefVec; i++)
	{
		dotProd[i]=refVec[i]*vecOut;
		if(true==watch)
		{
			YsPrintf("  %s dotProd=%lf L=%lf\n",refVec[i].Txt(),dotProd[i],refVec[i].GetLength());
		}
		if(0==i || dotProd[i]<minDot)
		{
			minDot=dotProd[i];
		}
	}



	for(;;) // Until no more improvement can be found.
	{
		if(YSOK!=vecOut.Normalize())
		{
			return YSERR;
		}

		YSBOOL stepImprovement=YSFALSE;
		// Can be two strategies:
		//   (1) Move toward the least similar direction.
		//   (2) Move away from the most similar direction, or move to the average of non-most-similar direction.


		for(int i=0; i<2; ++i)
		{
			YsVec3 goal(0.0,0.0,0.0);
			if(0==i) // Strategy (1)
			{
				for(int i=0; i<nRefVec; i++)
				{
					const double dotProd=refVec[i]*vecOut;
					if(YSTRUE==YsEqual(minDot,dotProd))
					{
						goal+=refVec[i];
					}
				}
			}
			else if(1==i) // Strategy (2)
			{
				double maxDot=0.0;
				for(int i=0; i<nRefVec; i++)
				{
					YsMakeGreater(maxDot,refVec[i]*vecOut);
				}
				for(int i=0; i<nRefVec; i++)
				{
					const double dotProd=refVec[i]*vecOut;
					if(YSTRUE!=YsEqual(maxDot,dotProd))
					{
						goal+=refVec[i];
					}
				}
			}

			if(YSOK!=goal.Normalize())
			{
				return YSERR;
			}

			if(true==watch)
			{
				YsPrintf("Goal: %s\n",goal.Txt());
			}

			ApplyConstraint(goal,nConst,constVec);

			double newMinimum;
			if(YSOK==OptimizeByBiSection(newMinimum,vecOut,goal,nRefVec,refVec,nConst,constVec,minDot) &&
			   fabs(minDot)*0.01<(newMinimum-minDot))  // 1% cut off
			{
				if(true==watch)
				{
					YsPrintf("Improvement=%s (%lf->%lf)\n",vecIn.Txt(),minDot,newMinimum);
				}

				minDot=newMinimum;
				improvement=YSOK;
				stepImprovement=YSTRUE;
			}
		}

		if(YSTRUE!=stepImprovement)
		{
			if(true==watch)
			{
				YsPrintf("No more improvement\n");
			}
			break;
		}
	}

	return improvement;
}

/* static */ YSRESULT YsPositiveVectorCalculator::OptimizeByBiSection(
    double &newMinimum,YsVec3 &vecInOut,const YsVec3 &vecGoal,const YSSIZE_T nRefVec,const YsVec3 refVec[],int /*nConst*/,const YsVec3 /*constVec*/[],const double currentMinimum)
{
	double t0,t1,f0,f1;
	t0=0.0;
	t1=1.0;

	f0=currentMinimum;
	f1=YsInfinity;
	for(int i=0; i<nRefVec; i++)
	{
		f1=YsSmaller(f1,refVec[i]*vecGoal);
	}

	for(int i=0; i<10; i++)
	{
		const double tm=(t0+t1)/2.0;
		YsVec3 vm=vecInOut*(1.0-tm)+vecGoal*tm;
		vm.Normalize();

		double fm=YsInfinity;
		for(int i=0; i<nRefVec; i++)
		{
			fm=YsSmaller(fm,refVec[i]*vm);
		}

		if(f0>f1)
		{
			f1=fm;
			t1=tm;
		}
		else
		{
			f0=fm;
			t0=tm;
		}
	}

	const double t=(f0>f1 ? t0 : t1);
	if(YSTRUE!=YsEqual(t,0.0))
	{
		YsVec3 newVec=vecInOut*(1.0-t)+vecGoal*t;
		if(YSOK==newVec.Normalize())
		{
			newMinimum=YsGreater(f0,f1);
			vecInOut=newVec;
			return YSOK;
		}
	}
	return YSERR;
}

/* static */YSRESULT YsPositiveVectorCalculator::ApplyConstraint(YsVec3 &vecInOut,int nConst,const YsVec3 constVec[])
{
	// Not really correct if 1<nConst
	if(0<nConst)
	{
		for(int i=0; i<nConst; ++i)
		{
			if(0.0>vecInOut*constVec[i])
			{
				vecInOut-=constVec[i]*(constVec[i]*vecInOut);
			}
		}
		vecInOut.Normalize();
	}
	return YSOK;
}



////////////////////////////////////////////////////////////



void YsPositiveVolumeCalculator::Initialize(const YsVec3 &min,const YsVec3 &max)
{
	shl.CleanUp();

	const YsShell::VertexHandle cubeVtHd[8]=
	{
		shl.AddVertexH(min),
		shl.AddVertexH(YsVec3(min.x(),max.y(),min.z())),
		shl.AddVertexH(YsVec3(max.x(),max.y(),min.z())),
		shl.AddVertexH(YsVec3(max.x(),min.y(),min.z())),

		shl.AddVertexH(YsVec3(min.x(),min.y(),max.z())),
		shl.AddVertexH(YsVec3(min.x(),max.y(),max.z())),
		shl.AddVertexH(max),
		shl.AddVertexH(YsVec3(max.x(),min.y(),max.z())),
	};

	const YsShell::VertexHandle cubePlHd[6][4]=
	{
		{cubeVtHd[0],cubeVtHd[1],cubeVtHd[2],cubeVtHd[3]},

		{cubeVtHd[1],cubeVtHd[0],cubeVtHd[4],cubeVtHd[5]},
		{cubeVtHd[2],cubeVtHd[1],cubeVtHd[5],cubeVtHd[6]},
		{cubeVtHd[3],cubeVtHd[2],cubeVtHd[6],cubeVtHd[7]},
		{cubeVtHd[0],cubeVtHd[3],cubeVtHd[7],cubeVtHd[4]},

		{cubeVtHd[7],cubeVtHd[6],cubeVtHd[5],cubeVtHd[4]},
	};

	const YsVec3 cubePlNom[6]=
	{
		YsVec3( 0.0, 0.0,-1.0),

		YsVec3(-1.0, 0.0, 0.0),
		YsVec3( 0.0, 1.0, 0.0),
		YsVec3( 1.0, 0.0, 0.0),
		YsVec3( 0.0,-1.0, 0.0),

		YsVec3( 0.0, 0.0, 1.0),
	};
	for(YSSIZE_T idx=0; idx<6; ++idx)
	{
		auto plHd=shl.AddPolygonH(4,cubePlHd[idx]);
		shl.SetNormalOfPolygon(plHd,cubePlNom[idx]);
	}
}
void YsPositiveVolumeCalculator::AddPlane(const YsPlane &pln)
{
	YsArray <YsShellVertexHandle,16> vtHdInsert,vtHdOnPlane;
	CalculateVertexOnEdge(vtHdInsert,vtHdOnPlane,pln);
	InsertVertexOnEdge(vtHdInsert);

	SlashPolygon(pln);
	DeleteNegativePolygon(pln);
	CloseHole(pln,vtHdInsert,vtHdOnPlane);
}

YSRESULT YsPositiveVolumeCalculator::TestPlane(const YsPlane &pln) const
{
	for(auto vtHd : shl.AllVertex())
	{
		if(0<pln.GetSideOfPlane(shl.GetVertexPosition(vtHd)))
		{
			return YSOK;
		}
	}
	return YSERR;
}

const YsShell &YsPositiveVolumeCalculator::GetVolume(void) const
{
	return shl;
}

YsVec3 YsPositiveVolumeCalculator::GetCenterOfGravity(void) const
{
	YsVec3 sum=YsVec3::Origin();
	double totalWeight=0.0;

	// In this case, the shell is strictly convex.  Also all polygons are strictly convex, too.  Polygon normals are also pre-calculated.

	YsVec3 cen=shl.GetShellCenter();
	for(auto plHd : shl.AllPolygon())
	{
		auto nom=shl.GetNormal(plHd);
		auto plVtPos=shl.GetPolygonVertexPosition(plHd);
		for(YSSIZE_T idx=1; idx<plVtPos.size()-1; ++idx)
		{
			auto area=YsGetTriangleArea3(plVtPos[0],plVtPos[idx],plVtPos[idx+1]);
			auto height=fabs((plVtPos[0]-cen)*nom);
			auto w=area*height/3.0;
			auto cgTet=(plVtPos[0]+plVtPos[idx]+plVtPos[idx+1]+cen)/4.0;

			sum+=w*cgTet;
			totalWeight+=w;
		}
	}

	if(0.0<totalWeight)
	{
		return sum/totalWeight;
	}
	return sum;
}
