#include "ysshellext_crawlingutil.h"



YsShellExt_EdgeCrawler::YsShellExt_EdgeCrawler()
{
	canPassEdge=nullptr;
}



/* static */ YsShellCrawler::State YsShellExt_EdgeCrawler::Create(const YsShellExt &shl,YsShellVertexHandle startVtHd)
{
	return YsShellCrawler::Create(shl.Conv(),startVtHd);
}

void YsShellExt_EdgeCrawler::SetCanPassFunc(const CanPassFunction *func)
{
	canPassEdge=func;
}

YSRESULT YsShellExt_EdgeCrawler::Start(const YsShellExt &shl,const YsVec3 &startDir,YsShellVertexHandle startVtHd)
{
	currentPlHd=nullptr;
	currentEdVtHd[0]=startVtHd;
	currentEdVtHd[1]=startVtHd;
	currentState=YsShellCrawler::STATE_ON_VERTEX;
	currentPos=shl.GetVertexPosition(startVtHd);
	currentDir=startDir;
	return YSOK;
}

YSRESULT YsShellExt_EdgeCrawler::Restart(const YsShellExt &,State sta)
{
	if(YsShellCrawler::STATE_ON_EDGE!=sta.state &&
	   YsShellCrawler::STATE_ON_VERTEX!=sta.state)
	{
		return YSERR;
	}

	YsShellCrawler::Initialize();

	currentPlHd=nullptr;
	currentEdVtHd[0]=sta.edVtHd[0];
	currentEdVtHd[1]=sta.edVtHd[1];
	currentState=sta.state;
	currentPos=sta.pos;
	currentDir=sta.dir;
	return YSOK;
}

YSRESULT YsShellExt_EdgeCrawler::Crawl(const YsShellExt &shl,const double &dist,YSBOOL watch)
{
	double distRemain=dist;
	while(0.0<distRemain || YSTRUE==YsEqual(dist,0.0))
	{
		if(this->crawlingMode==YsShellCrawler::CRAWL_TO_DIRECTION)
		{
			if(currentEdVtHd[0]==currentEdVtHd[1])
			{
				auto nextVtHd=PossibleNextVertexForDirection(shl,currentEdVtHd[0],this->currentDir);
				if(1!=nextVtHd.size())
				{
					reachedDeadEnd=YSTRUE;
					return YSOK;
				}

				currentState=YsShellCrawler::STATE_ON_EDGE;
				currentEdVtHd[1]=nextVtHd[0];
				currentDir=shl.GetEdgeVector(currentEdVtHd[0],currentEdVtHd[1]);
			}

			const double dToNext=(shl.GetVertexPosition(currentEdVtHd[1])-currentPos).GetLength();
			if(dToNext<distRemain)
			{
				distRemain-=dToNext;
				auto nextVtHdCandidate=PossibleNextVertex(shl,currentEdVtHd[1]);
				for(auto idx : nextVtHdCandidate.ReverseIndex())
				{
					if(nextVtHdCandidate[idx]==currentEdVtHd[0])
					{
						nextVtHdCandidate.DeleteBySwapping(idx);
					}
				}

				if(1==nextVtHdCandidate.size())
				{
					currentEdVtHd[0]=currentEdVtHd[1];
					currentEdVtHd[1]=nextVtHdCandidate[0];
					currentDir=shl.GetEdgeVector(currentEdVtHd[0],currentEdVtHd[1]);
				}
				else
				{
					reachedDeadEnd=YSTRUE;
					return YSOK;
				}
			}
			else
			{
				currentDir=shl.GetEdgeVector(currentEdVtHd[0],currentEdVtHd[1]);
				currentPos+=currentDir*distRemain;
				distRemain=0.0;
			}
		}
		else if(this->crawlingMode==YsShellCrawler::CRAWL_TO_GOAL)
		{
			if(currentEdVtHd[0]==currentEdVtHd[1])
			{
				if(shl.GetVertexPosition(currentEdVtHd[0])==goal)
				{
					reachedGoal=YSTRUE;
					return YSOK;
				}

				auto distSq=(shl.GetVertexPosition(currentEdVtHd[0])-goal).GetSquareLength();
				auto nextVtHd=PossibleNextVertexTowardGoal(shl,currentEdVtHd[0],this->goal);
				if(1<nextVtHd.size()) // More than one possibility.  Cannot decide.
				{
					reachedDeadEnd=YSTRUE;
					return YSOK;
				}
				if(0==nextVtHd.size()) // None gets closer to the goal.
				{
					reachedDeadEnd=YSTRUE;
					reachedNearGoal=YSTRUE;
					return YSOK;
				}

				currentState=YsShellCrawler::STATE_ON_EDGE;
				currentEdVtHd[1]=nextVtHd[0];
			}

			const YsVec3 edVtPos[2]=
			{
				shl.GetVertexPosition(currentEdVtHd[0]),
				shl.GetVertexPosition(currentEdVtHd[1]),
			};
			auto nearPos=YsGetNearestPointOnLine3(edVtPos,this->goal);
			if(YSTRUE==YsCheckInBetween3(nearPos,edVtPos))
			{
				this->currentPos=nearPos;
				if(nearPos==this->goal)
				{
					reachedGoal=YSTRUE;
				}
				else
				{
					reachedDeadEnd=YSTRUE;
					reachedNearGoal=YSTRUE;
				}
				return YSOK;
			}

			const double d[2]=
			{
				(edVtPos[0]-goal).GetSquareLength(),
				(edVtPos[1]-goal).GetSquareLength(),
			};
			auto nextVtHd0=(d[0]<d[1] ? currentEdVtHd[0] : currentEdVtHd[1]);
			distRemain-=(currentPos-shl.GetVertexPosition(nextVtHd0)).GetLength();
			currentPos=shl.GetVertexPosition(nextVtHd0);

			auto nextVtHd1Candidate=PossibleNextVertexTowardGoal(shl,nextVtHd0,this->goal);
			if(1==nextVtHd1Candidate.size())
			{
				currentEdVtHd[0]=nextVtHd0;
				currentEdVtHd[1]=nextVtHd1Candidate[0];
			}
			else if(0==nextVtHd1Candidate.size())
			{
				reachedDeadEnd=YSTRUE;
				reachedNearGoal=YSTRUE;
				currentEdVtHd[0]=nextVtHd0;
				currentEdVtHd[1]=nextVtHd0;
				return YSOK;
			}
			else
			{
				reachedDeadEnd=YSTRUE;
				currentEdVtHd[0]=nextVtHd0;
				currentEdVtHd[1]=nextVtHd0;
				return YSOK;
			}
		}

		if(YSTRUE==YsEqual(dist,0.0))
		{
			return YSOK;
		}
	}
	return YSOK;
}

YsArray <YsShell::VertexHandle,2> YsShellExt_EdgeCrawler::PossibleNextVertex(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd) const
{
	YsArray <YsShell::VertexHandle,2> nextVtHd;
	for(auto vtHd : shl.GetConnectedVertex(fromVtHd))
	{
		YsShell::Edge edge(fromVtHd,vtHd);
		if(YSTRUE==canPassEdge->CanPassEdge(shl,edge))
		{
			nextVtHd.push_back(vtHd);
		}
	}
	for(auto vtHd : shl.GetConstEdgeConnectedVertex(fromVtHd))
	{
		YsShell::Edge edge(fromVtHd,vtHd);
		if(YSTRUE!=nextVtHd.IsIncluded(vtHd) && YSTRUE==canPassEdge->CanPassEdge(shl,edge))
		{
			nextVtHd.push_back(vtHd);
		}
	}
	return nextVtHd;
}

YsArray <YsShell::VertexHandle,2> YsShellExt_EdgeCrawler::PossibleNextVertexForDirection(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsVec3 &dir) const
{
	auto curPos=shl.GetVertexPosition(fromVtHd);

	YsArray <YsShell::VertexHandle,2> nextVtHd;
	for(auto vtHd : shl.GetConnectedVertex(fromVtHd))
	{
		auto nextDir=shl.GetVertexPosition(vtHd)-curPos;
		if(0.0<dir*nextDir)
		{
			YsShell::Edge edge(fromVtHd,vtHd);
			if(YSTRUE==canPassEdge->CanPassEdge(shl,edge))
			{
				nextVtHd.push_back(vtHd);
			}
		}
	}
	for(auto vtHd : shl.GetConstEdgeConnectedVertex(fromVtHd))
	{
		auto nextDir=shl.GetVertexPosition(vtHd)-curPos;
		if(0.0<dir*nextDir)
		{
			YsShell::Edge edge(fromVtHd,vtHd);
			if(YSTRUE!=nextVtHd.IsIncluded(vtHd) && YSTRUE==canPassEdge->CanPassEdge(shl,edge))
			{
				nextVtHd.push_back(vtHd);
			}
		}
	}
	return nextVtHd;
}

YsArray <YsShell::VertexHandle,2> YsShellExt_EdgeCrawler::PossibleNextVertexTowardGoal(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsVec3 &goal) const
{
	const auto curPos=shl.GetVertexPosition(fromVtHd);
	const auto toGoal=goal-curPos;

	YsArray <YsShell::VertexHandle,2> nextVtHd;
	for(auto vtHd : shl.GetConnectedVertex(fromVtHd))
	{
		const auto edgeVec=shl.GetVertexPosition(vtHd)-curPos;
		if(0.0<edgeVec*toGoal)
		{
			YsShell::Edge edge(fromVtHd,vtHd);
			if(YSTRUE==canPassEdge->CanPassEdge(shl,edge))
			{
				nextVtHd.push_back(vtHd);
			}
		}
	}
	for(auto vtHd : shl.GetConstEdgeConnectedVertex(fromVtHd))
	{
		const auto edgeVec=shl.GetVertexPosition(vtHd)-curPos;
		if(0.0<edgeVec*toGoal)
		{
			YsShell::Edge edge(fromVtHd,vtHd);
			if(YSTRUE!=nextVtHd.IsIncluded(vtHd) && YSTRUE==canPassEdge->CanPassEdge(shl,edge))
			{
				nextVtHd.push_back(vtHd);
			}
		}
	}
	return nextVtHd;
}
