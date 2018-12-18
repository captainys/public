/* ////////////////////////////////////////////////////////////

File Name: ysshellext_patchutil.cpp
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

#include "ysshellext_patchutil.h"
#include "ysshellext_localop.h"
#include "ysshellext_geomcalc.h"
#include "ysshellext_orientationutil.h"

YsShellExt_PatchBetweenTwoConstEdge::YsShellExt_PatchBetweenTwoConstEdge()
{
	shl.EnableSearch();
}

YsShellExt_PatchBetweenTwoConstEdge::~YsShellExt_PatchBetweenTwoConstEdge()
{
}

void YsShellExt_PatchBetweenTwoConstEdge::CleanUp(void)
{
	shl.CleanUp();
	patchVtKeyToSrcVtHd.CleanUp();
	patchVtKeyToSeqId.CleanUp();
	seqCeHd[0]=NULL;
	seqCeHd[1]=NULL;

	rawVtHdSeq[0].CleanUp();
	rawVtHdSeq[1].CleanUp();
	vtHdSeq[0].CleanUp();
	vtHdSeq[1].CleanUp();
	totalLen[0]=0.0;
	totalLen[1]=0.0;
}

YSRESULT YsShellExt_PatchBetweenTwoConstEdge::SetVertexSequence(const YsShell &srcShl,
                       YSSIZE_T nVt0,const YsShellVertexHandle vtHdArray0[],YSBOOL isLoop0,
                       YSSIZE_T nVt1,const YsShellVertexHandle vtHdArray1[],YSBOOL isLoop1)
{
	return SetVertexSequence(srcShl,
		YsConstArrayMask<YsShell::VertexHandle>(nVt0,vtHdArray0),YsArray<YsVec3>(),isLoop0,
		YsConstArrayMask<YsShell::VertexHandle>(nVt1,vtHdArray1),YsArray<YsVec3>(),isLoop1);
}

YSRESULT YsShellExt_PatchBetweenTwoConstEdge::SetVertexSequence(const YsShell &srcShl,
	const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray0,const YsConstArrayMask <YsVec3> &vtPosArray0,YSBOOL isLoop0,
	const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray1,const YsConstArrayMask <YsVec3> &vtPosArray1,YSBOOL isLoop1)
{
	auto nVt0=vtHdArray0.size();
	auto nVt1=vtHdArray1.size();
	if((YSTRUE!=isLoop0 && 2>nVt0) || (YSTRUE==isLoop0 && 3>nVt0) ||
	   (YSTRUE!=isLoop1 && 2>nVt1) || (YSTRUE==isLoop1 && 3>nVt1))
	{
		return YSERR;
	}

	YsArray <YsShellVertexHandle> newVtHdArray[2];
	newVtHdArray[0].Set(nVt0,vtHdArray0);
	newVtHdArray[1].Set(nVt1,vtHdArray1);
	shl.CleanUp();

	isLoop[0]=isLoop0;
	isLoop[1]=isLoop1;

	{
		YsHashTable <YsShell::VertexHandle> srcVtKeyToNewVtHd;
		for(int seq=0; seq<2; ++seq)
		{
			const auto nVt=newVtHdArray[seq].size();
			const auto vtHdArray=newVtHdArray[seq].data();
			const auto vtPosArray=(0==seq ? vtPosArray0 : vtPosArray1);
			
			for(YSSIZE_T idx=0; idx<nVt; ++idx)
			{
				YsShell::VertexHandle vtHd=nullptr;
				if(nullptr!=vtHdArray[idx] && 
				   YSOK==srcVtKeyToNewVtHd.FindElement(vtHd,srcShl.GetSearchKey(vtHdArray[idx])) &&
				   nullptr!=vtHd)
				{
				}
				else
				{
					if(nullptr!=vtHdArray[idx])
					{
						vtHd=shl.AddVertex(srcShl.GetVertexPosition(vtHdArray[idx]));
						patchVtKeyToSrcVtHd.AddElement(shl.GetSearchKey(vtHd),vtHdArray[idx]);
						srcVtKeyToNewVtHd.AddElement(srcShl.GetSearchKey(vtHdArray[idx]),vtHd);
					}
					else
					{
						vtHd=shl.AddVertex(vtPosArray[idx]);
					}
				}
				int registeredSeqId=-1;
				if(YSOK==patchVtKeyToSeqId.FindElement(registeredSeqId,shl.GetSearchKey(vtHd)) &&
				   registeredSeqId!=seq)
				{
					patchVtKeyToSeqId.UpdateElement(shl.GetSearchKey(vtHd),-1);
				}
				else
				{
					patchVtKeyToSeqId.UpdateElement(shl.GetSearchKey(vtHd),seq);
				}
				rawVtHdSeq[seq].Append(vtHd);
			}
		}
	}

	if(YSOK==YsExtractNonSharedPart<YsShell::VertexHandle>(rawVtHdSeq[0],rawVtHdSeq[1],rawVtHdSeq[0],isLoop[0],rawVtHdSeq[1],isLoop[1]))
	{
		isLoop[0]=YSFALSE;
		isLoop[1]=YSFALSE;
	}

	// Did I assign patchVtKeyToSeqId right? >>
	for(int seq=0; seq<2; ++seq)
	{
		printf("Sequence %d\n",seq);
		for(auto vtHd : rawVtHdSeq[seq])
		{
			int registeredSeqId=-1;
			patchVtKeyToSeqId.FindElement(registeredSeqId,shl.GetSearchKey(vtHd));
			printf("%d\n",registeredSeqId);
		}
	}
	// Did I assign patchVtKeyToSeqId right? <<

	for(int seq=0; seq<2; ++seq)
	{
		totalLen[seq]=0.0;
		for(YSSIZE_T idx=1; idx<rawVtHdSeq[seq].size(); ++idx)
		{
			totalLen[seq]+=shl.GetEdgeLength(rawVtHdSeq[seq][idx-1],rawVtHdSeq[seq][idx]);
		}
		if(YSTRUE==isLoop[seq] && 3<=rawVtHdSeq[seq].GetN())
		{
			totalLen[seq]+=shl.GetEdgeLength(rawVtHdSeq[seq][0],rawVtHdSeq[seq].Last());
		}

		vtHdSeq[seq]=rawVtHdSeq[seq];
		seqCeHd[seq]=shl.AddConstEdge(rawVtHdSeq[seq],isLoop[seq]);
	}

	return YSOK;
}

YSRESULT YsShellExt_PatchBetweenTwoConstEdge::UpdateVertexSequence(int seqIdx,const YsShell &srcShl,const YsConstArrayMask <YsShell::VertexHandle> &srcVtHdArray)
{
	YsArray <YsShell::VertexHandle> vtHdStock=srcVtHdArray;
	for(auto vtHd : vtHdSeq[seqIdx])
	{
		YsShell::VertexHandle srcVtHd=nullptr;
		if(YSOK!=patchVtKeyToSrcVtHd.FindElement(srcVtHd,shl.GetSearchKey(vtHd)))
		{
			YSSIZE_T nearestIdx=-1;
			double nearDist=0.0;
			for(YSSIZE_T idx=0; idx<vtHdStock.size(); ++idx)
			{
				auto canVtHd=vtHdStock[idx];
				auto dist=(shl.GetVertexPosition(vtHd)-srcShl.GetVertexPosition(canVtHd)).GetSquareLength();
				if(0>nearestIdx || dist<nearDist)
				{
					nearDist=dist;
					nearestIdx=idx;
				}
			}
			if(0<=nearestIdx)
			{
				patchVtKeyToSrcVtHd.UpdateElement(shl.GetSearchKey(vtHd),vtHdStock[nearestIdx]);
				vtHdStock.DeleteBySwapping(nearestIdx);
			}
		}
	}
	return YSOK;
}

void YsShellExt_PatchBetweenTwoConstEdge::CalculateParameter(void)
{
	for(int seq=0; seq<2; ++seq)
	{
		paramArray[seq].Set(vtHdSeq[seq].GetN(),NULL);
		if(YSTRUE==isLoop[seq])
		{
			paramArray[seq].Increment();
		}

		paramArray[seq][0]=0.0;
		paramArray[seq].Last()=1.0;

		double lSum=0.0;

		int last=(YSTRUE==isLoop[seq] ? 1 : 0);
		for(YSSIZE_T idx=1; idx<vtHdSeq[seq].GetN()-1+last; ++idx)
		{
			lSum+=shl.GetEdgeLength(vtHdSeq[seq][idx-1],vtHdSeq[seq].GetCyclic(idx));
			paramArray[seq][idx]=lSum/totalLen[seq];
		}

		printf("Sequence %d\n",seq);
		for(auto t : paramArray[seq])
		{
			printf("%lf\n",t);
		}
	}
}

void YsShellExt_PatchBetweenTwoConstEdge::MakeInitialGuess(void)
{
	YSSIZE_T index[2]={0,0};

	int last[2]=
	{
		(YSTRUE==isLoop[0] ? 1 : 0),
		(YSTRUE==isLoop[1] ? 1 : 0),
	};
	while(index[0]<paramArray[0].GetN()-1 || index[1]<paramArray[1].GetN()-1)
	{
		int nextMove=-1;
		if(index[0]>=paramArray[0].GetN()-1)
		{
			nextMove=1;
		}
		else if(index[1]>=paramArray[1].GetN()-1)
		{
			nextMove=0;
		}
		else if(paramArray[0][index[0]]<paramArray[1][index[1]])
		{
			nextMove=0;
		}
		else 
		{
			nextMove=1;
		}

		YsShellVertexHandle triVtHd[3]=
		{
			vtHdSeq[0].GetCyclic(index[0]),
			vtHdSeq[1].GetCyclic(index[1]),
			vtHdSeq[nextMove].GetCyclic(index[nextMove]+1)
		};
		if(triVtHd[0]!=triVtHd[1] && triVtHd[1]!=triVtHd[2] && triVtHd[0]!=triVtHd[2])
		{
			shl.AddPolygon(3,triVtHd);
		}

		++index[nextMove];
	}
}

void YsShellExt_PatchBetweenTwoConstEdge::MinimizeDihedralAngleSum(void)
{
	YsArray <YsStaticArray <YsShellVertexHandle,2> > allEdge;

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);
		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			allEdge.Increment();
			allEdge.Last()[0]=edVtHd[0];
			allEdge.Last()[1]=edVtHd[1];
		}
	}

	for(;;)
	{
		YSBOOL improvement=YSFALSE;

		for(auto &edVtHd : allEdge)
		{
			YsShell_SwapInfo swapInfo;
			if(YSOK==swapInfo.MakeInfo((const YsShell &)shl,edVtHd))
			{
				if(0<shl.GetNumPolygonUsingEdge(swapInfo.newDiagonal[0],swapInfo.newDiagonal[1]))
				{
					continue;
				}

				int seqId[2];
				if(YSOK!=patchVtKeyToSeqId.FindElement(seqId[0],shl.GetSearchKey(swapInfo.newDiagonal[0])) ||
				   YSOK!=patchVtKeyToSeqId.FindElement(seqId[1],shl.GetSearchKey(swapInfo.newDiagonal[1])) ||
				   seqId[0]==seqId[1] ||
				   seqId[0]<0 ||  // seqId[?] will be negative if the vertex is used from multiple sequences.
				   seqId[1]<0)
				{
					continue;
				}

				const double oldDhaTotal=YsShellExt_CalculateTotalDihedralAngleAroundEdge(shl,swapInfo.orgDiagonal);

				YsArray <YsShellVertexHandle,4> oldTriVtHd[2];
				shl.GetPolygon(oldTriVtHd[0],swapInfo.triPlHd[0]);
				shl.GetPolygon(oldTriVtHd[1],swapInfo.triPlHd[1]);

				shl.SetPolygonVertex(swapInfo.triPlHd[0],3,swapInfo.newTriVtHd[0]);
				shl.SetPolygonVertex(swapInfo.triPlHd[1],3,swapInfo.newTriVtHd[1]);

				const double newDhaTotal=YsShellExt_CalculateTotalDihedralAngleAroundEdge(shl,swapInfo.newDiagonal);

				if(newDhaTotal<oldDhaTotal-YsTolerance)
				{
					improvement=YSTRUE;
					edVtHd[0]=swapInfo.newDiagonal[0];
					edVtHd[1]=swapInfo.newDiagonal[1];
				}
				else
				{
					shl.SetPolygonVertex(swapInfo.triPlHd[0],oldTriVtHd[0]);
					shl.SetPolygonVertex(swapInfo.triPlHd[1],oldTriVtHd[1]);
				}
			}
		}

		if(YSTRUE!=improvement)
		{
			break;
		}
	}
}

void YsShellExt_PatchBetweenTwoConstEdge::MergeTriFormQuad(void)
{
	// Greedy method
	YsArray <YsShell::Edge> edgeArray;
	YsArray <double> maxDotArray;

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		auto edge=shl.GetEdge(edHd);

		YsShell_MergeInfo mergeInfo;
		auto edPlHd=shl.FindPolygonFromEdgePiece(edge);
		if(2==edPlHd.GetN() && YSOK==mergeInfo.MakeInfo(shl.Conv(),edPlHd))
		{
			auto quadVtHd=mergeInfo.GetNewPolygon();
			if(4==quadVtHd.GetN())
			{
				int seqId[2];
				if(YSOK!=patchVtKeyToSeqId.FindElement(seqId[0],shl.GetSearchKey(quadVtHd[0])) ||
				   YSOK!=patchVtKeyToSeqId.FindElement(seqId[1],shl.GetSearchKey(quadVtHd[2])) ||
				   seqId[0]==seqId[1] ||
				   seqId[0]<0 ||  // seqId[?] will be negative if the vertex is used from multiple sequences.
				   seqId[1]<0)
				{
					continue;
				}
				if(YSOK!=patchVtKeyToSeqId.FindElement(seqId[0],shl.GetSearchKey(quadVtHd[1])) ||
				   YSOK!=patchVtKeyToSeqId.FindElement(seqId[1],shl.GetSearchKey(quadVtHd[3])) ||
				   seqId[0]==seqId[1] ||
				   seqId[0]<0 ||  // seqId[?] will be negative if the vertex is used from multiple sequences.
				   seqId[1]<0)
				{
					continue;
				}

				// 2017/04/20 Quad-quality was checked by the shape quality.  However, for this purpose, I think
				//            plane-ness should be used.
				YsVec3 n[4];
				for(int i=0; i<4; ++i)
				{
					YsVec3 v1=shl.GetVertexPosition(quadVtHd.GetCyclic(i+1))-shl.GetVertexPosition(quadVtHd[i]);
					YsVec3 v2=shl.GetVertexPosition(quadVtHd.GetCyclic(i+2))-shl.GetVertexPosition(quadVtHd.GetCyclic(i+1));
					n[i]=YsUnitVector(v1^v2);
				}
				double minDot=0.0;
				for(int i=0; i<4; ++i)
				{
					for(int j=i+1; j<4; ++j)
					{
						YsMakeSmaller(minDot,fabs(n[i]*n[j]));
					}
				}
				edgeArray.Add(edge);
				maxDotArray.Add(-minDot);
			}
		}
	NEXTEDGE:
		;
	}

	YsSimpleMergeSort<double,YsShell::Edge>(maxDotArray.GetN(),maxDotArray,edgeArray);

	for(auto edge : edgeArray)
	{
		YsShell_MergeInfo mergeInfo;
		auto edPlHd=shl.FindPolygonFromEdgePiece(edge);
		if(2==edPlHd.GetN() && YSOK==mergeInfo.MakeInfo(shl.Conv(),edPlHd))
		{
			auto quadVtHd=mergeInfo.GetNewPolygon();
			if(4==quadVtHd.GetN())
			{
				shl.SetPolygonVertex(edPlHd[0],quadVtHd);
				shl.DeletePolygon(edPlHd[1]);
			}
		}
	}
}

YsArray <YsShell::VertexHandle> YsShellExt_PatchBetweenTwoConstEdge::GetVertexSequence(int seq) const
{
	YsArray <YsShell::VertexHandle> vtHdArray;
	for(auto vtHd : vtHdSeq[seq])
	{
		YsShell::VertexHandle srcVtHd=nullptr;
		patchVtKeyToSrcVtHd.FindElement(srcVtHd,shl.GetSearchKey(vtHd));
		vtHdArray.push_back(srcVtHd);
	}
	return vtHdArray;
}



////////////////////////////////////////////////////////////

void YsShellExt_MultiPatchSequence::CleanUp(void)
{
	for(auto &patch : patchUtil)
	{
		patch.CleanUp();
	}
	patchUtil.CleanUp();
}

const YsConstArrayMask<YsShellExt_PatchBetweenTwoConstEdge> YsShellExt_MultiPatchSequence::GetPatchSequence(void) const
{
	YsConstArrayMask <YsShellExt_PatchBetweenTwoConstEdge> mask=patchUtil;
	return mask;
}

YSRESULT YsShellExt_MultiPatchSequence::SetUpFromConstEdge(const YsShellExt &srcShl,const YsConstArrayMask<YsShellExt::ConstEdgeHandle> &ceHdArray)
{
	if(2>ceHdArray.size())
	{
		return YSERR;
	}

	patchUtil.Set(ceHdArray.GetN()-1,NULL);

	for(YSSIZE_T idx=0; idx<ceHdArray.GetN()-1; ++idx)
	{
		YsArray <YsShellVertexHandle> ceVtHd[2];
		YSBOOL isLoop[2];

		srcShl.GetConstEdge(ceVtHd[0],isLoop[0],ceHdArray[idx]);
		srcShl.GetConstEdge(ceVtHd[1],isLoop[1],ceHdArray[idx+1]);

		patchUtil[idx].CleanUp();
		if(YSOK!=patchUtil[idx].SetVertexSequence(srcShl.Conv(),ceVtHd,isLoop))
		{
			return YSERR;
		}
		patchUtil[idx].CalculateParameter();
		patchUtil[idx].MakeInitialGuess();
		patchUtil[idx].MinimizeDihedralAngleSum();
	}
	CalculateNormal();
	return YSOK;
}

YSRESULT YsShellExt_MultiPatchSequence::SetUpFromPolygon(const YsShellExt &srcShl,const YsConstArrayMask<YsShell::PolygonHandle> &inPlHd)
{
	if(2>inPlHd.size())
	{
		return YSERR;
	}	

	patchUtil.Set(inPlHd.GetN()-1,NULL);

	for(YSSIZE_T idx=0; idx<inPlHd.GetN()-1; ++idx)
	{
		YsArray <YsShellVertexHandle> plVtHd[2];
		const YSBOOL isLoop[2]={YSTRUE,YSTRUE};

		srcShl.GetPolygon(plVtHd[0],inPlHd[idx]);
		srcShl.GetPolygon(plVtHd[1],inPlHd[idx+1]);

		patchUtil[idx].CleanUp();
		if(YSOK!=patchUtil[idx].SetVertexSequence(srcShl.Conv(),plVtHd,isLoop))
		{
			return YSERR;
		}
		patchUtil[idx].CalculateParameter();
		patchUtil[idx].MakeInitialGuess();
		patchUtil[idx].MinimizeDihedralAngleSum();
	}
	CalculateNormal();
	return YSOK;
}

YSRESULT YsShellExt_MultiPatchSequence::SetUpFromFirstLastCrossSectionAndPathBetween(
	    const YsShellExt &srcShl,
	    const YsConstArrayMask <YsShell::VertexHandle> &seg0In,
	    const YsConstArrayMask <YsShell::VertexHandle> &seg1In,
	    const YsConstArrayMask <YsShell::VertexHandle> &pathIn)
{
	YsArray <YsShell::VertexHandle> seg[2]=
	{
		seg0In,seg1In
	};

	YsArray <YsShell::VertexHandle> path=pathIn;
	if(YSTRUE==seg[0].IsIncluded(path.front()) && YSTRUE==seg[1].IsIncluded(path.back()))
	{
		path.pop_back();
	}
	if(YSTRUE==seg[0].IsIncluded(path.back()) && YSTRUE==seg[1].IsIncluded(path.front()))
	{
		path.Invert();
		path.pop_back();
	}
	else if(YSTRUE==seg[0].IsIncluded(path.front()) && YSTRUE!=seg[1].IsIncluded(path.back()))
	{
		// In this case, nothing to do.
	}
	else if(YSTRUE==seg[0].IsIncluded(path.back()) && YSTRUE!=seg[1].IsIncluded(path.front()))
	{
		path.Invert();
	}

	const YsVec3 axsVtPos[2]=
	{
		srcShl.GetVertexPosition(seg[0].front()),
		srcShl.GetVertexPosition(seg[0].back())
	};
	auto xAxs=YsUnitVector(axsVtPos[1]-axsVtPos[0]);
	YsVec3 origin;
	YsVec3 yAxs;
	double L0;
	YsGetNearestPointOnLine3(origin,axsVtPos[0],axsVtPos[1],srcShl.GetVertexPosition(path[0]));
	yAxs=srcShl.GetVertexPosition(path[0])-origin;
	L0=yAxs.GetLength();
	yAxs/=L0;

	YSSIZE_T indexOfPathInSeg=-1;
	for(YSSIZE_T i=0; i<seg[0].size(); ++i)
	{
		if(seg[0][i]==path[0])
		{
			indexOfPathInSeg=i;
			break;
		}
	}
	if(0>=indexOfPathInSeg)
	{
		return YSERR;
	}

	YsArray <YsShell::VertexHandle> prevSeg=seg[0];
	YsArray <YsVec3> prevSegPos;
	patchUtil.resize(path.size());
	for(YSSIZE_T i=0; i<path.size(); ++i)
	{
		YsArray <YsShell::VertexHandle> nextSeg;
		YsArray <YsVec3> nextSegPos;
		if(i==path.size()-1)
		{
			nextSeg=seg[1];
		}
		else
		{
			YsVec3 nextYAxs=srcShl.GetVertexPosition(path[i+1])-origin;
			double L=nextYAxs.GetLength();
			nextYAxs/=L;
			double yScale=L/L0;

			nextSeg.resize(seg[0].size());
			nextSegPos.resize(seg[0].size());
			for(YSSIZE_T idx=0; idx<seg[0].size(); ++idx)
			{
				auto v=srcShl.GetVertexPosition(seg[0][idx]);
				double x=(v-origin)*xAxs;
				double y=(v-origin)*yAxs;

				nextSeg[idx]=nullptr;
				nextSegPos[idx]=origin+x*xAxs+y*nextYAxs*yScale;
			}
			nextSeg[0]=seg[0].front();
			nextSeg[indexOfPathInSeg]=path[i+1];
			nextSeg.back()=seg[0].back();
		}

		patchUtil[i].SetVertexSequence(srcShl.Conv(),prevSeg,prevSegPos,YSFALSE,nextSeg,nextSegPos,YSFALSE);
		patchUtil[i].CalculateParameter();
		patchUtil[i].MakeInitialGuess();
		patchUtil[i].MinimizeDihedralAngleSum();

		prevSeg=nextSeg;
		prevSegPos=nextSegPos;
	}

	CalculateNormal();
	return YSOK;
}

void YsShellExt_MultiPatchSequence::CalculateNormal(void)
{
	for(auto &patch : patchUtil)
	{
		YsShellExt_OrientationUtil orientationUtil;
		orientationUtil.CleanUp();
		orientationUtil.FixOrientationOfClosedSolid(patch.shl);
		auto plHdNeedFip=orientationUtil.GetPolygonNeedFlip();

		for(auto plHd : plHdNeedFip)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			patch.shl.GetPolygon(plVtHd,plHd);
			plVtHd.Invert();
			patch.shl.SetPolygonVertex(plHd,plVtHd);
		}

		orientationUtil.RecalculateNormalFromCurrentOrientation((const YsShell &)patch.shl);
		auto plNomPairNeedNormalReset=orientationUtil.GetPolygonNormalPair();
		for(auto plNomPair : plNomPairNeedNormalReset)
		{
			patch.shl.SetPolygonNormal(plNomPair.plHd,plNomPair.nom);
		}
	}
}

void YsShellExt_MultiPatchSequence::Invert(void)
{
	for(auto &patch : patchUtil)
	{
		YsShellExt_OrientationUtil::InvertPolygonAll(patch.shl);
	}
}

void YsShellExt_MultiPatchSequence::MergeTriFormQuad(void)
{
	for(auto &patch : patchUtil)
	{
		patch.MergeTriFormQuad();
	}
}
