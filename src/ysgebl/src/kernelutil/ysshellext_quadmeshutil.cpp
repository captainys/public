#include <thread>

#include "ysshellext_localop.h"
#include "ysshellext_quadmeshutil.h"



/* virtual */ double YsShellExt_QuadMeshUtil::ScoreFunc_Orthogonality::Calculate(const YsShellExt &shl,const Candidate &can) const
{
	double maxDev=0.0;

	// Max deviation from PI/2
	for(int i=0; i<4; ++i)
	{
		const YsShell::VertexHandle vtHd[3]=
		{
			can.quadVtHd[(i+3)&3],
			can.quadVtHd[i],
			can.quadVtHd[(i+1)&3]
		};
		YsVec3 vec[2]=
		{
			shl.GetEdgeVector(vtHd[1],vtHd[0]),
			shl.GetEdgeVector(vtHd[1],vtHd[2])
		};
		if(YSOK!=vec[0].Normalize() || YSOK!=vec[1].Normalize())
		{
			maxDev=1.0;
			break;
		}
		else
		{
			YsMakeGreater(maxDev,fabs(vec[0]*vec[1]));
		}
	}
	return maxDev;
}
/* virtual */ double YsShellExt_QuadMeshUtil::ScoreFunc_Orthogonality::Threshold(const YsShellExt &shl) const
{
	return YsCos45deg;
}

////////////////////////////////////////////////////////////

YsShellExt_QuadMeshUtil::YsShellExt_QuadMeshUtil()
{
}
YsShellExt_QuadMeshUtil::~YsShellExt_QuadMeshUtil()
{
	CleanUp();
}
void YsShellExt_QuadMeshUtil::CleanUp(void)
{
}

void YsShellExt_QuadMeshUtil::ThreadParam::Run(void)
{
	for(auto edHd : toTest)
	{
		YsShell_MergeInfo info;
		auto edPlHd=shlPtr->FindPolygonFromEdgePiece(edHd);
		if(2==edPlHd.size() &&
		   3==shlPtr->GetPolygonNumVertex(edPlHd[0]) &&
		   3==shlPtr->GetPolygonNumVertex(edPlHd[1]) &&
		   YSOK==info.MakeInfo(shlPtr->Conv(),edPlHd.data()))
		{
			auto quadVtHd=info.GetNewPolygon();
			if(4==quadVtHd.size())
			{
				Candidate can;
				can.quadVtHd[0]=quadVtHd[0];
				can.quadVtHd[1]=quadVtHd[1];
				can.quadVtHd[2]=quadVtHd[2];
				can.quadVtHd[3]=quadVtHd[3];
				can.srcPlKey[0]=shlPtr->GetSearchKey(edPlHd[0]);
				can.srcPlKey[1]=shlPtr->GetSearchKey(edPlHd[1]);

				auto score=scoreFuncPtr->Calculate(*shlPtr,can);
				if(score<scoreFuncPtr->Threshold(*shlPtr))
				{
					this->candidate.push_back(can);
					this->score.push_back(score);
				}
			}
		}
	}
}

void YsShellExt_QuadMeshUtil::MakeCandidate(const YsShellExt &shl,const ScoreFunc &scoreFunc,int nThread)
{
	CleanUp();

	if(nThread<1)
	{
		nThread=1;
	}
	YsArray <ThreadParam> thrParam(nThread,nullptr);
	for(auto &p : thrParam)
	{
		p.shlPtr=&shl;
		p.scoreFuncPtr=&scoreFunc;
	}
	{
		int i=0;
		for(auto edHd : shl.AllEdge())
		{
			thrParam[i].toTest.push_back(edHd);
			i=(i+1)%nThread;
		}
	}

	YsArray <std::thread> thr;
	thr.resize(nThread);
	for(int i=0; i<nThread; ++i)
	{
		std::thread t(&YsShellExt_QuadMeshUtil::ThreadParam::Run,&thrParam[i]);
		std::swap(thr[i],t);
	}

	for(auto &t : thr)
	{
		t.join();
	}


	YsArray <Candidate> allCandidate;
	YsArray <double> allScore;
	for(auto &t : thrParam)
	{
		allCandidate.Append(t.candidate);
		allScore.Append(t.score);
	}
	YsSimpleMergeSort <double,Candidate> (allScore.size(),allScore.data(),allCandidate.data());
}
