/* ////////////////////////////////////////////////////////////

File Name: ysdelaunaytri.cpp
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

#include "ysdelaunaytri.h"


YsUnconstrainedDelaunayTriangulation::Edge YsUnconstrainedDelaunayTriangulation::Triangle::GetEdge(int edIdx) const
{
	Edge edge;
	edge.ndIdx[0]=ndIdx[edIdx];
	edge.ndIdx[1]=ndIdx[(edIdx+1)%3];
	return edge;
}

////////////////////////////////////////////////////////////

int YsUnconstrainedDelaunayTriangulation::CreateTri(void)
{
	int newTriIdx;
	if(0<unusedTriIdx.GetN())
	{
		newTriIdx=(int)unusedTriIdx.Last();
		unusedTriIdx.DeleteLast();
	}
	else
	{
		newTriIdx=(int)tri.GetN();
		tri.Increment();
	}

	auto &newTri=tri[newTriIdx];
	newTri.alive=YSTRUE;
	newTri.neiTriIdx[0]=-1;
	newTri.neiTriIdx[1]=-1;
	newTri.neiTriIdx[2]=-1;
	newTri.testedCounter=0;
	newTri.cavityCounter=0;
	newTri.visitedCounter=0;

	lastAddedTriIndex=newTriIdx;

	return newTriIdx;
}

void YsUnconstrainedDelaunayTriangulation::FalsifyAll(void) const
{
	++trueFalseThreshold;
}

YSBOOL YsUnconstrainedDelaunayTriangulation::HasBeenDelaunayTested(const Triangle &tri) const
{
	return (trueFalseThreshold<=tri.testedCounter ? YSTRUE : YSFALSE);
}

YSBOOL YsUnconstrainedDelaunayTriangulation::IsIncludedInCavity(const Triangle &tri) const
{
	return (trueFalseThreshold<=tri.cavityCounter ? YSTRUE : YSFALSE);
}

YSBOOL YsUnconstrainedDelaunayTriangulation::HasBeenVisitedForEnclosingTriSearch(const Triangle &tri) const
{
	return (trueFalseThreshold<=tri.visitedCounter ? YSTRUE : YSFALSE);
}

void YsUnconstrainedDelaunayTriangulation::MarkDelaunayTested(Triangle &tri) const
{
	tri.testedCounter=trueFalseThreshold;
}
void YsUnconstrainedDelaunayTriangulation::MarkIncludedInCavity(Triangle &tri) const
{
	tri.cavityCounter=trueFalseThreshold;
}
void YsUnconstrainedDelaunayTriangulation::MarkVisitedForEnclosingTriSearch(const Triangle &tri) const
{
	tri.visitedCounter=trueFalseThreshold;
}

void YsUnconstrainedDelaunayTriangulation::DeleteTri(YSSIZE_T triIdx)
{
	tri[triIdx].alive=YSFALSE;
	unusedTriIdx.Append(triIdx);
}

void YsUnconstrainedDelaunayTriangulation::ConnectTri(int fromTriIdx,int toTriIdx)
{
	if(0<=fromTriIdx && 0<=toTriIdx)
	{
		auto &fromTri=tri[fromTriIdx];
		auto &toTri=tri[toTriIdx];

		for(int fromEdIdx=0; fromEdIdx<3; ++fromEdIdx)
		{
			auto fromEdge=fromTri.GetEdge(fromEdIdx);
			for(int toEdIdx=0; toEdIdx<3; ++toEdIdx)
			{
				auto toEdge=toTri.GetEdge(toEdIdx);
				if(YSTRUE==YsSameEdge(fromEdge.ndIdx,toEdge.ndIdx))
				{
					fromTri.neiTriIdx[fromEdIdx]=toTriIdx;
					toTri.neiTriIdx[toEdIdx]=fromTriIdx;
					return;
				}
			}
		}
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Trying to connect two triangles that are not neighbors.\n");
	}
}

void YsUnconstrainedDelaunayTriangulation::DeleteTriAndClearConnection(YSSIZE_T  triIdx)
{
	if(0<=triIdx)
	{
		auto &toDel=tri[triIdx];
		if(YSTRUE==toDel.alive)
		{
			DisconnectTri(triIdx);
			DeleteTri(triIdx);
		}
	}
}

void YsUnconstrainedDelaunayTriangulation::DisconnectTri(YSSIZE_T triIdx)
{
	if(0<=triIdx)
	{
		auto &toDel=tri[triIdx];
		if(YSTRUE==toDel.alive)
		{
			for(int neiIdx=0; neiIdx<3; ++neiIdx)
			{
				auto neiTriIdx=toDel.neiTriIdx[neiIdx];
				if(0<=neiTriIdx)
				{
					DisconnectTri(tri[neiTriIdx],triIdx);
				}
			}
		}
	}
}

void YsUnconstrainedDelaunayTriangulation::DisconnectTri(Triangle &from,YSSIZE_T triIdxToCut)
{
	if(0<=triIdxToCut)
	{
		for(int idx=0; idx<3; ++idx)
		{
			if(from.neiTriIdx[idx]==triIdxToCut)
			{
				from.neiTriIdx[idx]=-1;
			}
		}
	}
}

YsUnconstrainedDelaunayTriangulation::YsUnconstrainedDelaunayTriangulation()
{
	CleanUp();
}

YsUnconstrainedDelaunayTriangulation::~YsUnconstrainedDelaunayTriangulation()
{
}

void YsUnconstrainedDelaunayTriangulation::CleanUp(void)
{
	node.CleanUp();
	tri.CleanUp();
	unusedTriIdx.CleanUp();
	lastAddedTriIndex=-1;
	trueFalseThreshold=1;
}

int YsUnconstrainedDelaunayTriangulation::AddInitialNode(const YsVec2 &pos,int intAttrib)
{
	const int ndIdx=(int)node.GetN();
	node.Increment();
	node.Last().pos=pos;
	node.Last().intAttrib=intAttrib;
	node.Last().isInitialNode=YSTRUE;
	return ndIdx;
}

int YsUnconstrainedDelaunayTriangulation::AddInitialTri(const int ndIdx[3])
{
	const int newTriIdx=CreateTri();

	auto &newTri=tri[newTriIdx];
	newTri.ndIdx[0]=ndIdx[0];
	newTri.ndIdx[1]=ndIdx[1];
	newTri.ndIdx[2]=ndIdx[2];


	YsVec2 v1=node[ndIdx[1]].pos-node[ndIdx[0]].pos;
	YsVec2 v2=node[ndIdx[2]].pos-node[ndIdx[1]].pos;
	if((v1^v2)<0.0)
	{
		YsSwapSomething(newTri.ndIdx[1],newTri.ndIdx[2]);
	}

	for(int newTriEdIdx=0; newTriEdIdx<3; ++newTriEdIdx)
	{
		const Edge newTriEdge=newTri.GetEdge(newTriEdIdx);

		for(int neiTriIdx=0; neiTriIdx<tri.GetN()-1; ++neiTriIdx)
		{
			auto &neiTri=tri[neiTriIdx]; // neiTri: Neighbor triangle candidate.
			for(int neiTriEdIdx=0; neiTriEdIdx<3; ++neiTriEdIdx)
			{
				const Edge neiTriEdge=neiTri.GetEdge(neiTriEdIdx);
				if(YSTRUE==YsSameEdge(newTriEdge.ndIdx,neiTriEdge.ndIdx))
				{
					newTri.neiTriIdx[newTriEdIdx]=neiTriIdx;
					neiTri.neiTriIdx[neiTriEdIdx]=newTriIdx;
				}
			}
		}
	}

	return newTriIdx;
}

int YsUnconstrainedDelaunayTriangulation::AddInitialTri(int ndIdx0,int ndIdx1,int ndIdx2)
{
	const int ndIdx[3]=
	{
		ndIdx0,ndIdx1,ndIdx2
	};
	return AddInitialTri(ndIdx);
}

YSSIZE_T YsUnconstrainedDelaunayTriangulation::GetNumNode(void) const
{
	return node.GetN();
}

const YsVec2 YsUnconstrainedDelaunayTriangulation::GetNodePos(YSSIZE_T ndIdx) const
{
	return node[ndIdx].pos;
}

YSSIZE_T YsUnconstrainedDelaunayTriangulation::GetNumTri(void) const
{
	return tri.GetN();
}

YSRESULT YsUnconstrainedDelaunayTriangulation::GetTriangleNodePos(YsVec2 ndPos[3],YSSIZE_T triIdx) const
{
	if(YSTRUE==tri.IsInRange(triIdx) && YSTRUE==tri[triIdx].alive)
	{
		auto &t=tri[triIdx];
		ndPos[0]=node[t.ndIdx[0]].pos;
		ndPos[1]=node[t.ndIdx[1]].pos;
		ndPos[2]=node[t.ndIdx[2]].pos;
		return YSOK;
	}
	return YSERR;
}

YsVec2 YsUnconstrainedDelaunayTriangulation::GetTriangleCenter(int triIdx) const
{
	auto &t=tri[triIdx];
	return (node[t.ndIdx[0]].pos+node[t.ndIdx[1]].pos+node[t.ndIdx[2]].pos)/3.0;
}

int YsUnconstrainedDelaunayTriangulation::FindEnclosingTriangleIndex(const YsVec2 &pos) const
{
	if(0==tri.GetN())
	{
		return -1;
	}

	FalsifyAll();

	YSSIZE_T curTriIdx=FindStartingTriangle();
	if(0>curTriIdx)
	{
		return -1;
	}

	for(;;)
	{
		YsVec2 triNdPos[3];
		GetTriangleNodePos(triNdPos,curTriIdx);

		auto side=YsCheckInsideTriangle2(pos,triNdPos);
		if(YSINSIDE==side || YSBOUNDARY==side)
		{
			return (int)curTriIdx;
		}

		const YSSIZE_T nextTriIdx=StepToNextTriangle(curTriIdx,pos);
		if(0>nextTriIdx)
		{
			break;
		}
		curTriIdx=nextTriIdx;
	}

	// Not found.  Go exhaustive search.
	static int flipper=1;
	flipper=1-flipper;
	printf("Triangle not found.  Going exhaustive. %d\n",flipper);
	for(int curTriIdx=0; curTriIdx<tri.GetN(); ++curTriIdx)
	{
		if(YSTRUE==tri[curTriIdx].alive)
		{
			YsVec2 triNdPos[3];
			GetTriangleNodePos(triNdPos,curTriIdx);

			auto side=YsCheckInsideTriangle2(pos,triNdPos);
			if(YSINSIDE==side || YSBOUNDARY==side)
			{
				return curTriIdx;
			}
		}
	}

	return -1;
}

int YsUnconstrainedDelaunayTriangulation::FindTriangleUsingNode(YSSIZE_T ndIdx) const
{
	if(0==tri.GetN())
	{
		return -1;
	}

	FalsifyAll();

	YSSIZE_T curTriIdx=FindStartingTriangle();
	if(0>curTriIdx)
	{
		return -1;
	}

	const auto goalPos=node[ndIdx].pos;

	for(;;)
	{
		auto &t=tri[curTriIdx];
		if(t.ndIdx[0]==ndIdx || t.ndIdx[1]==ndIdx || t.ndIdx[2]==ndIdx)
		{
			return (int)curTriIdx;
		}

		const YSSIZE_T nextTriIdx=StepToNextTriangle(curTriIdx,goalPos);
		if(0>nextTriIdx)
		{
			break;
		}
		curTriIdx=nextTriIdx;
	}

	printf("%s %d\n",__FUNCTION__,__LINE__);
	printf("  Triangle not found from vtx %d.  Go exhaustive search.\n",(int)ndIdx);
	for(auto curTriIdx : tri.AllIndex())
	{
		if(YSTRUE==tri[curTriIdx].alive)
		{
			auto &t=tri[curTriIdx];
			if(t.ndIdx[0]==ndIdx || t.ndIdx[1]==ndIdx || t.ndIdx[2]==ndIdx)
			{
				return (int)curTriIdx;
			}
		}
	}

	return -1;
}

int YsUnconstrainedDelaunayTriangulation::FindFirstTriangleForBuildingPipe(YSSIZE_T ndIdx,const YsVec2 &goalPos) const
{
	const auto triIdx0=FindTriangleUsingNode(ndIdx);
	if(0<=triIdx0)
	{
		const auto pos0=GetNodePos(ndIdx);

		YsArray <int,16> toTest;
		toTest.Append(triIdx0);

		FalsifyAll();
		MarkVisitedForEnclosingTriSearch(tri[triIdx0]);

		while(0<toTest.GetN())
		{
			auto t=toTest[0];

			int triNdIdx[3];
			GetTriangleNodeIndex(triNdIdx,t);
			for(int edIdx=0; edIdx<3; ++edIdx)
			{
				if(triNdIdx[edIdx]!=ndIdx && triNdIdx[(edIdx+1)%3]!=ndIdx)
				{
					YsVec2 crs;
					const YsVec2 edVtPos[2]=
					{
						GetNodePos(triNdIdx[edIdx]),
						GetNodePos(triNdIdx[(edIdx+1)%3])
					};
					if(YSOK==YsGetLineIntersection2(crs,pos0,goalPos,edVtPos[0],edVtPos[1]) &&
					   YSTRUE==YsCheckInBetween2(crs,pos0,goalPos) &&
					   YSTRUE==YsCheckInBetween2(crs,edVtPos))
					{
						return t;
					}
				}
				else
				{
					auto neiTri=GetNeighborTriangleIndex(t,edIdx);
					if(YSTRUE!=HasBeenVisitedForEnclosingTriSearch(tri[neiTri]))
					{
						MarkVisitedForEnclosingTriSearch(tri[neiTri]);
						toTest.Append(neiTri);
					}
				}
			}

			toTest.DeleteBySwapping(0);
		}
	}
	return -1;
}

YsArray <int> YsUnconstrainedDelaunayTriangulation::FindPipe(YSSIZE_T ndIdx0,YSSIZE_T ndIdx1) const
{
	YsArray <int> pipe;
	const auto triIdx0=FindFirstTriangleForBuildingPipe(ndIdx0,GetNodePos(ndIdx1));
	if(0<=triIdx0)
	{
		const YsVec2 path[2]=
		{
			GetNodePos(ndIdx0),
			GetNodePos(ndIdx1)
		};

		FalsifyAll();

		MarkVisitedForEnclosingTriSearch(tri[triIdx0]);
		pipe.Append(triIdx0);
		for(;;) // Until reaching ndIdx1
		{
			const int curTriIdx=pipe.Last();
			int triNdIdx[3];
			GetTriangleNodeIndex(triNdIdx,curTriIdx);
			if(triNdIdx[0]==ndIdx1 || triNdIdx[1]==ndIdx1 || triNdIdx[2]==ndIdx1)
			{
				return pipe;
			}

			YSSIZE_T nextTriIdx=-1;

			YsVec2 triNdPos[3];
			GetTriangleNodePos(triNdPos,curTriIdx);
			for(int edIdx=0; edIdx<3; ++edIdx)
			{
				auto neiTriIdx=GetNeighborTriangleIndex(curTriIdx,edIdx);
				if(0<=neiTriIdx && YSTRUE!=HasBeenVisitedForEnclosingTriSearch(tri[neiTriIdx]))
				{
					const YSSIZE_T edNdIdx[2]=
					{
						triNdIdx[edIdx],triNdIdx[(edIdx+1)%3]
					};
					if(edNdIdx[0]!=ndIdx0 && edNdIdx[1]!=ndIdx0)
					{
						const YsVec2 edVtPos[2]=
						{
							GetNodePos(edNdIdx[0]),
							GetNodePos(edNdIdx[1])
						};
						YsVec2 crs;

						if(YSOK==YsGetLineIntersection2(crs,path[0],path[1],edVtPos[0],edVtPos[1]) &&
						   YSTRUE==YsCheckInBetween2(crs,path) &&
						   YSTRUE==YsCheckInBetween2(crs,edVtPos))
						{
							nextTriIdx=neiTriIdx;
							break;
						}
					}
				}
			}

			if(0<=nextTriIdx)
			{
				MarkVisitedForEnclosingTriSearch(tri[nextTriIdx]);
				pipe.Append((int)nextTriIdx);
			}
			else
			{
				return pipe;
			}
		}
	}
	pipe.CleanUp();
	return pipe;
}



YSRESULT YsUnconstrainedDelaunayTriangulation::ReducePipe(YsArray <int> &pipe,YSSIZE_T ndIdx0,YSSIZE_T ndIdx1)
{
	YSRESULT didSwap=YSERR;
//printf("NPipe In %d\n",pipe.GetN());
//for(auto p : pipe)printf(" %d",p);
//printf("\n");
	const YsVec2 path[2]={GetNodePos(ndIdx0),GetNodePos(ndIdx1)};
	const YsVec2 pathVec=YsUnitVector(path[1]-path[0]);
	const YsVec2 pathNom(pathVec.y(),-pathVec.x());

	if(1<pipe.GetN())
	{
		YSSIZE_T idx=0;
		while(idx<pipe.GetN()-1)
		{
			SwapInfo info;
			if(YSOK==MakeSwapInfo(info,pipe[idx],pipe[idx+1]) &&
			   YSTRUE==IsSwappable(info))
			{
				const YSSIZE_T newEdNdIdx[2]={info.q[0],info.q[2]};
				const YsVec2 newEdVtPos[2]={GetNodePos(newEdNdIdx[0]),GetNodePos(newEdNdIdx[1])};

				YsVec2 crs;
				if(newEdNdIdx[0]==ndIdx0 ||
				   newEdNdIdx[0]==ndIdx1 ||
				   newEdNdIdx[1]==ndIdx0 ||
				   newEdNdIdx[1]==ndIdx1 ||
				   YSOK!=YsGetLineIntersection2(crs,path[0],path[1],newEdVtPos[0],newEdVtPos[1]) ||
				   YSTRUE!=YsCheckInBetween2(crs,newEdVtPos[0],newEdVtPos[1]))
				{
					didSwap=YSOK;
					Swap(info);
					YSSIZE_T sharedEdNdIdx[2];
//printf("%s %d\n",__FUNCTION__,__LINE__);
//printf("  > %d %d %d (%d %d)\n",idx,idx+2,pipe.GetN(),pipe[idx],pipe[idx+1]);
//printf("    %d\n",(0<idx && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx-1],pipe[idx])));
//printf("    %d\n",(0<idx && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx-1],pipe[idx+1])));
//printf("    %d\n",(idx+2<pipe.GetN() && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx+2],pipe[idx])));
//printf("    %d\n",(idx+2<pipe.GetN() && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx+2],pipe[idx+1])));
					// Update pipe.  Delete one of pipe[idx] or pipe[idx+1].
					if((0<idx && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx-1],pipe[idx])) ||
					   (idx+2<pipe.GetN() && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx+2],pipe[idx])))
					{
						pipe.Delete(idx+1);
					}
					else
					{
						pipe.Delete(idx);
					}
				}
				else
				{
					// Heuristic:
					//   Probably, the height (perpendicular length of the edge) should
					//   become shorter.
					const YsVec2 dgnNew=GetNodePos(info.q[2])-GetNodePos(info.q[0]);
					const YsVec2 dgnOld=GetNodePos(info.q[3])-GetNodePos(info.q[1]);

					const double Lnew=fabs(pathNom*dgnNew);
					const double Lold=fabs(pathNom*dgnOld);

					if(Lnew<Lold-YsTolerance)
					{
						didSwap=YSOK;
						Swap(info);
//printf("%s %d\n",__FUNCTION__,__LINE__);

						YSSIZE_T sharedEdNdIdx[2];
						if((0<idx && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx-1],pipe[idx+1])) ||
						   (idx+2<pipe.GetN() && YSOK==GetSharedEdge(sharedEdNdIdx,pipe[idx+2],pipe[idx])))
						{
							YsSwapSomething(pipe[idx],pipe[idx+1]);
						}
					}

					++idx;
				}
			}
			else
			{
				++idx;
			}

//printf("%s %d\n",__FUNCTION__,__LINE__);
//printf("Checking Pipe Validity for Debugging Purpose\n");
//if(YSOK!=CheckPipeValidity(pipe))
//{
//	printf("Error!\n");
//	return YSERR;
//}
		}
	}

//printf("NPipe Out %d\n",pipe.GetN());
//for(auto p : pipe)printf(" %d",p);
//printf("\n");

	return didSwap;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::RecoverBoundaryEdge(YSSIZE_T ndIdx0,YSSIZE_T ndIdx1)
{
	auto pipe=FindPipe(ndIdx0,ndIdx1);
	while(YSOK==ReducePipe(pipe,ndIdx0,ndIdx1))
	{
	}
	if(1==pipe.GetN())
	{
		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsUnconstrainedDelaunayTriangulation::FindStartingTriangle(void) const
{
	YSSIZE_T curTriIdx=(YSTRUE==tri.IsInRange(lastAddedTriIndex) ? lastAddedTriIndex : 0);
	if(YSTRUE!=tri[curTriIdx].alive)
	{
		for(auto idx : tri.AllIndex())
		{
			if(YSTRUE==tri[idx].alive)
			{
				curTriIdx=idx;
				break;
			}
		}
	}
	if(YSTRUE!=tri[curTriIdx].alive)
	{
		return -1;
	}
	return curTriIdx;
}

YSSIZE_T YsUnconstrainedDelaunayTriangulation::StepToNextTriangle(YSSIZE_T curTriIdx,const YsVec2 &goalPos) const
{
	YsVec2 triNdPos[3];
	if(YSOK==GetTriangleNodePos(triNdPos,curTriIdx))
	{
		const YsVec2 edMidPos[3]=
		{
			(triNdPos[0]+triNdPos[1])/2.0,
			(triNdPos[1]+triNdPos[2])/2.0,
			(triNdPos[2]+triNdPos[0])/2.0
		};
		const YsVec2 edVec[3]=
		{
			triNdPos[1]-triNdPos[0],
			triNdPos[2]-triNdPos[1],
			triNdPos[0]-triNdPos[2]
		};
		const YsVec2 edNom[3]=
		{
			YsVec2(edVec[0].y(),-edVec[0].x()),
			YsVec2(edVec[1].y(),-edVec[1].x()),
			YsVec2(edVec[2].y(),-edVec[2].x())
		};

		for(int edIdx=0; edIdx<3; ++edIdx)
		{
			if(0.0<(goalPos-edMidPos[edIdx])*edNom[edIdx])
			{
				const int neiTriIdx=GetNeighborTriangleIndex(curTriIdx,edIdx);
				if(0<=neiTriIdx && YSTRUE!=HasBeenVisitedForEnclosingTriSearch(tri[neiTriIdx]))
				{
					MarkVisitedForEnclosingTriSearch(tri[neiTriIdx]);
					return neiTriIdx;
				}
			}
		}
	}
	return -1;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::CheckPipeValidity(YsArray <int> &pipe) const
{
	for(int i=0; i<pipe.GetN()-1; ++i)
	{
		if(0<=pipe[i])
		{
			for(int j=i+1; j<pipe.GetN(); ++j)
			{
				if(0<=pipe[j])
				{
					YSSIZE_T shared[2];
					if(YSOK!=GetSharedEdge(shared,pipe[i],pipe[j]))
					{
						return YSERR;
					}
					break;
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::MakeSwapInfo(SwapInfo &info,YSSIZE_T triIdx0,YSSIZE_T triIdx1) const
{
	YSSIZE_T sharedEdNdIdx[2];
	if(YSOK==GetSharedEdge(sharedEdNdIdx,triIdx0,triIdx1))
	{
		const auto &t=tri[triIdx0];

		YSSIZE_T quadNdIdx[4];
		YSSIZE_T quadNeiTriIdx[4];

		quadNdIdx[0]=t.ndIdx[0];
		quadNdIdx[1]=t.ndIdx[1];
		quadNdIdx[2]=t.ndIdx[2];

		quadNeiTriIdx[0]=t.neiTriIdx[0];
		quadNeiTriIdx[1]=t.neiTriIdx[1];
		quadNeiTriIdx[2]=t.neiTriIdx[2];

		while(YSTRUE!=YsSameEdge(quadNdIdx[1],quadNdIdx[2],sharedEdNdIdx[0],sharedEdNdIdx[1]))
		{
			auto swpNdIdx=quadNdIdx[0];
			quadNdIdx[0]=quadNdIdx[1];
			quadNdIdx[1]=quadNdIdx[2];
			quadNdIdx[2]=swpNdIdx;

			auto swpNeiTriIdx=quadNeiTriIdx[0];
			quadNeiTriIdx[0]=quadNeiTriIdx[1];
			quadNeiTriIdx[1]=quadNeiTriIdx[2];
			quadNeiTriIdx[2]=swpNeiTriIdx;
		}

		quadNdIdx[3]=quadNdIdx[2];
		quadNeiTriIdx[3]=quadNeiTriIdx[2];

		quadNdIdx[2]=GetNonSharedTriangleNodeIndex(triIdx1,sharedEdNdIdx);
		quadNeiTriIdx[1]=GetNeighborTriangleIndex(triIdx1,quadNdIdx[1],quadNdIdx[2]);
		quadNeiTriIdx[2]=GetNeighborTriangleIndex(triIdx1,quadNdIdx[2],quadNdIdx[3]);

		info.triIdx[0]=triIdx0;
		info.triIdx[1]=triIdx1;
		info.q[0]=quadNdIdx[0];
		info.q[1]=quadNdIdx[1];
		info.q[2]=quadNdIdx[2];
		info.q[3]=quadNdIdx[3];
		info.n[0]=quadNeiTriIdx[0];
		info.n[1]=quadNeiTriIdx[1];
		info.n[2]=quadNeiTriIdx[2];
		info.n[3]=quadNeiTriIdx[3];

		return YSOK;
	}
	return YSERR;
}

YSBOOL YsUnconstrainedDelaunayTriangulation::IsSwappable(const SwapInfo &info) const
{
	const YsVec2 quadVtPos[4]=
	{
		GetNodePos(info.q[0]),
		GetNodePos(info.q[1]),
		GetNodePos(info.q[2]),
		GetNodePos(info.q[3])
	};
	YsVec2 crs;
	if(YSOK==YsGetLineIntersection2(crs,quadVtPos[0],quadVtPos[2],quadVtPos[1],quadVtPos[3]) &&
	   YSTRUE==YsCheckInBetween2(crs,quadVtPos[0],quadVtPos[2]) &&
	   YSTRUE==YsCheckInBetween2(crs,quadVtPos[1],quadVtPos[3]) &&
	   quadVtPos[1]!=crs &&
	   quadVtPos[3]!=crs)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::Swap(SwapInfo &info)
{
	DisconnectTri(info.triIdx[0]);
	DisconnectTri(info.triIdx[1]);

	auto &t0=tri[info.triIdx[0]];
	t0.ndIdx[0]=(int)info.q[0];
	t0.ndIdx[1]=(int)info.q[1];
	t0.ndIdx[2]=(int)info.q[2];
	t0.neiTriIdx[0]=(int)info.n[0];
	t0.neiTriIdx[1]=(int)info.n[1];
	t0.neiTriIdx[2]=(int)info.triIdx[1];

	auto &t1=tri[info.triIdx[1]];
	t1.ndIdx[0]=(int)info.q[2];
	t1.ndIdx[1]=(int)info.q[3];
	t1.ndIdx[2]=(int)info.q[0];
	t1.neiTriIdx[0]=(int)info.n[2];
	t1.neiTriIdx[1]=(int)info.n[3];
	t1.neiTriIdx[2]=(int)info.triIdx[0];

	ConnectTri((int)info.triIdx[0],(int)info.n[0]);
	ConnectTri((int)info.triIdx[0],(int)info.n[1]);

	ConnectTri((int)info.triIdx[1],(int)info.n[2]);
	ConnectTri((int)info.triIdx[1],(int)info.n[3]);

	return YSOK;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::GetSharedEdge(YSSIZE_T edNdIdx[2],YSSIZE_T triIdx0,YSSIZE_T triIdx1) const
{
	if(0<=triIdx0 && YSTRUE==tri[triIdx0].alive &&
	   0<=triIdx1 && YSTRUE==tri[triIdx1].alive)
	{
		auto &tri0=tri[triIdx0];
		auto &tri1=tri[triIdx1];
		for(int i=0; i<3; ++i)
		{
			const YSSIZE_T ed0[2]=
			{
				tri0.ndIdx[i],
				tri0.ndIdx[(i+1)%3]
			};
			for(int j=0; j<3; ++j)
			{
				const YSSIZE_T ed1[2]=
				{
					tri1.ndIdx[j],
					tri1.ndIdx[(j+1)%3]
				};
				if(YSTRUE==YsSameEdge(ed0,ed1))
				{
					edNdIdx[0]=ed0[0];
					edNdIdx[1]=ed0[1];
					return YSOK;
				}
			}
		}
	}
	edNdIdx[0]=-1;
	edNdIdx[1]=-1;
	return YSERR;
}

YSSIZE_T YsUnconstrainedDelaunayTriangulation::GetNonSharedTriangleNodeIndex(YSSIZE_T triIdx,const YSSIZE_T edNdIdx[2]) const
{
	if(0<=triIdx)
	{
		auto &t=tri[triIdx];
		for(int i=0; i<3; ++i)
		{
			if(t.ndIdx[i]!=edNdIdx[0] && t.ndIdx[i]!=edNdIdx[1])
			{
				return t.ndIdx[i];
			}
		}
	}
	return -1;
}

int YsUnconstrainedDelaunayTriangulation::GetNeighborTriangleIndex(YSSIZE_T  fromTriIdx,int edIdx) const
{
	return tri[fromTriIdx].neiTriIdx[edIdx];
}

int YsUnconstrainedDelaunayTriangulation::GetNeighborTriangleIndex(YSSIZE_T fromTriIdx,YSSIZE_T edNdIdx0,YSSIZE_T edNdIdx1) const
{
	if(0<=fromTriIdx)
	{
		auto &t=tri[fromTriIdx];
		for(int edIdx=0; edIdx<3; ++edIdx)
		{
			if(YSTRUE==YsSameEdge<YSSIZE_T>(edNdIdx0,edNdIdx1,t.ndIdx[edIdx],t.ndIdx[(edIdx+1)%3]))
			{
				return GetNeighborTriangleIndex(fromTriIdx,edIdx);
			}
		}
	}
	return -1;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::GetCircumCircle(YsVec2 &cen,double &rad,int triIdx) const
{
	YsVec2 triPos[3];
	GetTriangleNodePos(triPos,triIdx);
	return YsGetCircumCircle(cen,rad,triPos[0],triPos[1],triPos[2]);
}

YSBOOL YsUnconstrainedDelaunayTriangulation::IsTriangleAlive(int triIdx) const
{
	return tri[triIdx].alive;
}

YsArray <int> YsUnconstrainedDelaunayTriangulation::BuildCavity(const YsVec2 &pos)
{
	YsArray <int> cavityTriIdx;

	const int tri0Idx=FindEnclosingTriangleIndex(pos);
	if(0<=tri0Idx)
	{
		YsVec2 triPos[3];
		GetTriangleNodePos(triPos,tri0Idx);
		if(triPos[0]==pos || triPos[1]==pos || triPos[2]==pos)
		{
			return cavityTriIdx;
		}

		FalsifyAll();
		MarkDelaunayTested(tri[tri0Idx]);
		MarkIncludedInCavity(tri[tri0Idx]);

		cavityTriIdx.Append(tri0Idx);
		for(YSSIZE_T ptr=0; ptr<cavityTriIdx.GetN(); ++ptr)
		{
			for(int edIdx=0; edIdx<3; ++edIdx)
			{
				auto neiTriIdx=GetNeighborTriangleIndex(cavityTriIdx[ptr],edIdx);
				if(0<=neiTriIdx && YSTRUE!=HasBeenDelaunayTested(tri[neiTriIdx]))
				{
					MarkDelaunayTested(tri[neiTriIdx]);
					YsVec2 cen;
					double rad;
					if(YSOK==GetCircumCircle(cen,rad,neiTriIdx) && (cen-pos).GetSquareLength()<YsSqr(rad+YsTolerance))
					{
						cavityTriIdx.Append(neiTriIdx);
						MarkIncludedInCavity(tri[neiTriIdx]);
					}
				}
			}
		}
	}

	return cavityTriIdx;
}

int YsUnconstrainedDelaunayTriangulation::InsertNode(const YsVec2 &pos,int intAttrib)
{
	auto cavity=BuildCavity(pos);
	if(0<cavity.GetN())
	{
		YsArray <EdgeAndTriangleIndex> cavityBoundary;

		for(auto triIdx : cavity)
		{
			for(int edIdx=0; edIdx<3; ++edIdx)
			{
				auto neiTriIdx=GetNeighborTriangleIndex(triIdx,edIdx);
				if(0>neiTriIdx || YSTRUE!=IsIncludedInCavity(tri[neiTriIdx]))
				{
					auto edge=tri[triIdx].GetEdge(edIdx);

					cavityBoundary.Increment();
					cavityBoundary.Last().ndIdx[0]=edge.ndIdx[0];
					cavityBoundary.Last().ndIdx[1]=edge.ndIdx[1];
					cavityBoundary.Last().triIdx=neiTriIdx;
				}
			}
		}

		for(auto triIdx : cavity)
		{
			DeleteTri(triIdx);
		}

		for(auto boundary : cavityBoundary)
		{
			node[boundary.ndIdx[0]].usingTriIdx=-1;
			node[boundary.ndIdx[1]].usingTriIdx=-1;
		}

		const int newNdIdx=(int)node.GetN();
		node.Increment();
		auto &newNode=node.Last();
		newNode.pos=pos;
		newNode.intAttrib=intAttrib;
		newNode.isInitialNode=YSFALSE;
		for(auto boundary : cavityBoundary)
		{
			auto newTriIdx=CreateTri();
			auto &newTri=tri[newTriIdx];

			newTri.ndIdx[0]=boundary.ndIdx[0];
			newTri.ndIdx[1]=boundary.ndIdx[1];
			newTri.ndIdx[2]=newNdIdx;

			newTri.neiTriIdx[0]=boundary.triIdx;
			// newTri.neiTriIdx[1]=Need to do something smart.
			// newTri.neiTriIdx[2]=

			for(int i=0; i<2; ++i)
			{
				if(node[boundary.ndIdx[i]].usingTriIdx<0)
				{
					node[boundary.ndIdx[i]].usingTriIdx=newTriIdx;
				}
				else
				{
					ConnectTri(node[boundary.ndIdx[i]].usingTriIdx,newTriIdx);
					node[boundary.ndIdx[i]].usingTriIdx=-1;
				}
			}

			auto neiTriIdx=boundary.triIdx;
			if(0<=neiTriIdx)
			{
				auto &neiTri=tri[neiTriIdx];
				for(int neiTriEdIdx=0; neiTriEdIdx<3; ++neiTriEdIdx)
				{
					auto neiTriEdge=neiTri.GetEdge(neiTriEdIdx);
					if(YSTRUE==YsSameEdge(neiTriEdge.ndIdx,boundary.ndIdx))
					{
						neiTri.neiTriIdx[neiTriEdIdx]=newTriIdx;
					}
				}
			}
		}

		return newNdIdx;
	}
	return -1;
}

void YsUnconstrainedDelaunayTriangulation::DeleteInitialTriangle(void)
{
	for(YSSIZE_T triIdx=0; triIdx<tri.GetN(); ++triIdx)
	{
		auto &t=tri[triIdx];
		if(YSTRUE==t.alive)
		{
			if(YSTRUE==node[t.ndIdx[0]].isInitialNode ||
			   YSTRUE==node[t.ndIdx[1]].isInitialNode ||
			   YSTRUE==node[t.ndIdx[2]].isInitialNode)
			{
				DeleteTriAndClearConnection(triIdx);
			}
		}
	}
}

YSRESULT YsUnconstrainedDelaunayTriangulation::GetTriangleNodeIndex(int ndIdx[3],YSSIZE_T triIdx) const
{
	auto &t=tri[triIdx];
	if(YSTRUE==t.alive)
	{
		ndIdx[0]=t.ndIdx[0];
		ndIdx[1]=t.ndIdx[1];
		ndIdx[2]=t.ndIdx[2];
		return YSOK;
	}
	return YSERR;
}
int YsUnconstrainedDelaunayTriangulation::GetNodeIntAttrib(YSSIZE_T ndIdx) const
{
	return node[ndIdx].intAttrib;
}

YSBOOL YsUnconstrainedDelaunayTriangulation::IsInitialNode(YSSIZE_T ndIdx) const
{
	return node[ndIdx].isInitialNode;
}

YSRESULT YsUnconstrainedDelaunayTriangulation::SaveSrf(const char fn[]) const
{
	FILE *fp=fopen(fn,"w");
	if(NULL!=fp)
	{
		fprintf(fp,"Surf\n");
		for(auto &n : node)
		{
			fprintf(fp,"V %lf %lf %lf\n",n.pos.x(),n.pos.y(),0.0);
		}
		for(auto &t : tri)
		{
			if(YSTRUE==t.alive)
			{
				fprintf(fp,"F\n");
				fprintf(fp,"V %d %d %d\n",t.ndIdx[0],t.ndIdx[1],t.ndIdx[2]);
			}
			fprintf(fp,"E\n");
		}
		fprintf(fp,"E\n");

		fclose(fp);
		return YSOK;
	}
	return YSERR;
}



#include "ysshell2d.h"
YSRESULT YsUnconstrainedDelaunayTriangulation::DeleteOutsideTriAfterEdgeRegistration(void)
{
	YsShell2d shl2d;

	YsHashTable <YsShell2dVertexHandle> ndIdxToVtHd;
	for(auto ndIdx : node.AllIndex())
	{
		auto &n=node[ndIdx];
		if(YSTRUE!=n.isInitialNode)
		{
			auto vtHd=shl2d.AddVertexH(n.pos);
			ndIdxToVtHd.Add((YSHASHKEY)ndIdx,vtHd);
		}
	}

	YsTwoKeyStore boundaryEdgeNode;
	for(YSSIZE_T edIdx=0; edIdx<=boundaryEdge.GetN()-2; edIdx+=2)
	{
		auto vtHdPtr0=ndIdxToVtHd.FindElement((YSHASHKEY)boundaryEdge[edIdx]);
		auto vtHdPtr1=ndIdxToVtHd.FindElement((YSHASHKEY)boundaryEdge[edIdx+1]);
		if(nullptr!=vtHdPtr0 && nullptr!=vtHdPtr1)
		{
			shl2d.AddEdgeH(*vtHdPtr0,*vtHdPtr1);
		}
		else
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Could be a serious implementation error.\n");
			printf("  I'm not supposed to see this message.\n");
		}
		boundaryEdgeNode.AddKey((YSHASHKEY)boundaryEdge[edIdx],(YSHASHKEY)boundaryEdge[edIdx+1]);
	}

	YsShell2dLattice ltc;
	ltc.SetDomain(shl2d,shl2d.GetNumEdge());

	YsKeyStore triMarkedInside,triMarkedOutside;
	for(int triIdx=0; triIdx<tri.GetN(); ++triIdx)
	{
		if(YSTRUE==IsTriangleAlive(triIdx) &&
		   YSTRUE!=triMarkedInside.IsIncluded(triIdx) &&
		   YSTRUE!=triMarkedOutside.IsIncluded(triIdx))
		{
			auto cen=GetTriangleCenter(triIdx);
			auto side=ltc.CheckInsidePolygon(cen);

			decltype(triMarkedOutside) *mark=nullptr;
			if(YSOUTSIDE==side)
			{
				mark=&triMarkedOutside;
			}
			else if(YSINSIDE==side)
			{
				mark=&triMarkedInside;
			}

			if(nullptr!=mark)
			{
				mark->Add(triIdx);

				YsArray <int> triToLookAt;
				triToLookAt.Add(triIdx);
				while(0<triToLookAt.GetN())
				{
					auto &t=tri[triToLookAt[0]];

					for(int edIdx=0; edIdx<3; ++edIdx)
					{
						const YSHASHKEY edNdIdx[2]={(YSHASHKEY)t.ndIdx[edIdx],(YSHASHKEY)t.ndIdx[(edIdx+1)%3]};

						if(YSTRUE!=boundaryEdgeNode.IsIncluded(edNdIdx[0],edNdIdx[1]))
						{
							auto neiTriIdx=GetNeighborTriangleIndex(triToLookAt[0],edIdx);
							if(0<=neiTriIdx &&
							   YSTRUE!=triMarkedInside.IsIncluded(neiTriIdx) &&
							   YSTRUE!=triMarkedOutside.IsIncluded(neiTriIdx))
							{
								mark->Add(neiTriIdx);
								triToLookAt.Add(neiTriIdx);
							}
						}
					}

					triToLookAt.DeleteBySwapping(0);
				}
			}
		}
	}

	for(auto triIdx : triMarkedOutside)
	{
		DeleteTriAndClearConnection(triIdx);
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(YSRESULT &res,const YsConstArrayMask <YsVec2> &plg)
{
	if(3==plg.size())
	{
		return YsArray <YSSIZE_T> ({0,1,2});
	}

	res=YSOK;
	YsArray <YSSIZE_T> triVtx;

	auto nVertex=plg.size();
	YsArray <int,16> idx;
	idx.resize(plg.size());

	for(auto i : idx.AllIndex())
	{
		idx[i]=(int)i;
	}

	// Delaunay triangulation starts with a set of initial triangles that encloses all the nodes.
	// The following block calculates the bounding box
	YsBoundingBoxMaker2 mkBbx;
	mkBbx.Make(nVertex,plg);
	YsVec2 bbx[2],cen;
	mkBbx.Get(bbx);
	cen=(bbx[0]+bbx[1])/2.0;

	// 2017/06/07 Using a square instead of using a rectangle that fits the bounding box.
	//            If a very high aspect-ratio polygon is being triangulated,
	//            it is numerically more stable.
	double bbxL=(bbx[1]-bbx[0]).GetLongestDimension()*1.5/2.0;
	bbx[0]=cen-YsVec2(bbxL,bbxL);
	bbx[1]=cen+YsVec2(bbxL,bbxL);



	// Adding initial nodes and triangles
	YsUnconstrainedDelaunayTriangulation delTri;
	delTri.AddInitialNode(bbx[0],-1);  // -1 is the integer attribute.  You may want to use as vertex IDs.
	delTri.AddInitialNode(YsVec2(bbx[1].x(),bbx[0].y()),-1);
	delTri.AddInitialNode(bbx[1],-1);
	delTri.AddInitialNode(YsVec2(bbx[0].x(),bbx[1].y()),-1);
	delTri.AddInitialTri(0,1,2);
	delTri.AddInitialTri(2,3,0);



	// Delaunay node-insertion
	YsArray <YSSIZE_T> nodeIdx;
	for(int i=0; i<nVertex; ++i)
	{
		nodeIdx.push_back(delTri.InsertNode(plg[i],idx[i]));
	}



	// Boundary Recovery
	YSRESULT recovery=YSOK;
	for(int i=0; i<nVertex; ++i)
	{
		if(YSOK!=delTri.RecoverBoundaryEdge(nodeIdx[i],nodeIdx[(i+1)%nVertex]))
		{
			printf("Boundary Recoveyr Failure!  Triangles may not be correct.\n");
			recovery=YSERR;
		}
	}

	YSBOOL reverse=YSFALSE;
	if(YSOK==recovery)
	{
		YsArray <YsArray <YSSIZE_T> > contour;
		contour.Increment();
		for(YSSIZE_T i=0; i<nodeIdx.size(); ++i)
		{
			contour[0].push_back(nodeIdx[i]);
			contour[0].push_back(nodeIdx.GetCyclic(i+1));
		}
		delTri.DeleteOutsideTri(contour);
		for(int i=0; i<delTri.GetNumTri(); ++i)
		{
			if(YSTRUE==delTri.IsTriangleAlive(i))  // For performance reasons, some deleted triangles may be taking an index.
			{
				int triNdIdx[3];
				delTri.GetTriangleNodeIndex(triNdIdx,i);
				triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[0]));
				triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[1]));
				triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[2]));

				if((triNdIdx[0]==nodeIdx[1] && triNdIdx[1]==nodeIdx[0]) ||
				   (triNdIdx[1]==nodeIdx[1] && triNdIdx[2]==nodeIdx[0]) ||
				   (triNdIdx[2]==nodeIdx[1] && triNdIdx[0]==nodeIdx[0]))
				{
					reverse=YSTRUE;
				}
			}
		}
	}
	else // Resort to exhaustive method.
	{
		delTri.DeleteInitialTriangle();
		for(int i=0; i<delTri.GetNumTri(); ++i)
		{
			if(YSTRUE==delTri.IsTriangleAlive(i))  // For performance reasons, some deleted triangles may be taking an index.
			{
				if(YSINSIDE==YsCheckInsidePolygon2(delTri.GetTriangleCenter(i),nVertex,plg))
				{
					int triNdIdx[3];
					delTri.GetTriangleNodeIndex(triNdIdx,i);
					triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[0]));
					triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[1]));
					triVtx.push_back(delTri.GetNodeIntAttrib(triNdIdx[2]));

					if((triNdIdx[0]==nodeIdx[1] && triNdIdx[1]==nodeIdx[0]) ||
					   (triNdIdx[1]==nodeIdx[1] && triNdIdx[2]==nodeIdx[0]) ||
					   (triNdIdx[2]==nodeIdx[1] && triNdIdx[0]==nodeIdx[0]))
					{
						reverse=YSTRUE;
					}
				}
			}
		}
		res=YSERR;
	}

	if(YSTRUE==reverse)
	{
		for(YSSIZE_T idx=0; idx<=triVtx.size()-3; idx+=3)
		{
			std::swap(triVtx[idx],triVtx[idx+2]);
		}
	}

	return triVtx;
}

YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(YSRESULT &res,const YsConstArrayMask <YsVec3> &plg)
{
	if(3==plg.size())
	{
		return YsArray <YSSIZE_T> ({0,1,2});
	}

	YsVec3 nom;
	if(YSOK==YsFindLeastSquareFittingPlaneNormal(nom,plg.size(),plg))
	{
		// The following block projects the polygon onto the XY-plane.
		YsMatrix3x3 toXY;
		toXY.MakeToXY(nom);

		YsArray <YsVec2> plg2d;
		for(int i=0; i<plg.size(); ++i)
		{
			plg2d.push_back((toXY*plg[i]).xy());
		}

		return YsTriangulatePolygonDelaunay(res,plg2d);
	}
	else
	{
		printf("Cannot find a normal vector for projection.\n");
	}

	YsArray <YSSIZE_T> empty;
	return empty;
}

YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(const YsConstArrayMask <YsVec2> &plg)
{
	YSRESULT res;
	return YsTriangulatePolygonDelaunay(res,plg);
}
YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(const YsConstArrayMask <YsVec3> &plg)
{
	YSRESULT res;
	return YsTriangulatePolygonDelaunay(res,plg);
}
