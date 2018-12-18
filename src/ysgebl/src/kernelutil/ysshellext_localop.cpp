/* ////////////////////////////////////////////////////////////

File Name: ysshellext_localop.cpp
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

#include <ysdelaunaytri.h>

#include "ysshellext_geomcalc.h"
#include "ysshellext_localop.h"
#include "ysshellext_deformationevaluator.h"



void YsShell_LocalOperation::FaceGroupSplit::CleanUp(void)
{
	fgHd=NULL;
	fgPlHdArray.CleanUp();
}



////////////////////////////////////////////////////////////

YsShell_SwapInfo::YsShell_SwapInfo()
{
	CleanUp();
}

void YsShell_SwapInfo::CleanUp(void)
{
	orgDiagonal[0]=NULL;
	orgDiagonal[1]=NULL;
	newDiagonal[0]=NULL;
	newDiagonal[1]=NULL;
	quadVtHd[0]=NULL;
	quadVtHd[1]=NULL;
	quadVtHd[2]=NULL;
	quadVtHd[3]=NULL;
	triPlHd[0]=NULL;
	triPlHd[1]=NULL;

	newTriVtHd[0][0]=NULL;
	newTriVtHd[0][1]=NULL;
	newTriVtHd[0][2]=NULL;
	newTriVtHd[1][0]=NULL;
	newTriVtHd[1][1]=NULL;
	newTriVtHd[1][2]=NULL;
}

YSRESULT YsShell_SwapInfo::MakeInfo(const YsShell &shl,const YsShell::Edge edge)
{
	return MakeInfo(shl,edge.edVtHd[0],edge.edVtHd[1]);
}

YSRESULT YsShell_SwapInfo::MakeInfo(const YsShell &shl,const YsShellVertexHandle edVtHd[2])
{
	return MakeInfo(shl,edVtHd[0],edVtHd[1]);
}

YSRESULT YsShell_SwapInfo::MakeInfo(const YsShell &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1)
{
	const YsShellSearchTable *search=shl.GetSearchTable();
	if(NULL!=search)
	{
		int nEdPl;
		const YsShellPolygonHandle *edPlHd;
		if(YSOK==search->FindPolygonListByEdge(nEdPl,edPlHd,shl,edVtHd0,edVtHd1) && 
		   2==nEdPl &&
		   3==shl.GetNumVertexOfPolygon(edPlHd[0]) &&
		   3==shl.GetNumVertexOfPolygon(edPlHd[1]) &&
		   edPlHd[0]!=edPlHd[1])
		{
			YsArray <YsShellVertexHandle,4> plVtHd0;
			shl.GetVertexListOfPolygon(plVtHd0,edPlHd[0]);

			for(int i=0; i<3 && YSTRUE!=YsSameEdge(plVtHd0[0],plVtHd0[2],edVtHd0,edVtHd1); ++i)
			{
				YsShellVertexHandle swp=plVtHd0[0];
				plVtHd0[0]=plVtHd0[1];
				plVtHd0[1]=plVtHd0[2];
				plVtHd0[2]=swp;
			}

			YsArray <YsShellVertexHandle,4> plVtHd1;
			shl.GetVertexListOfPolygon(plVtHd1,edPlHd[1]);
			for(int i=0; i<3; ++i)
			{
				if(plVtHd1[i]!=edVtHd0 && plVtHd1[i]!=edVtHd1)
				{
					quadVtHd[0]=plVtHd0[0];
					quadVtHd[1]=plVtHd0[1];
					quadVtHd[2]=plVtHd0[2];
					quadVtHd[3]=plVtHd1[i];

					orgDiagonal[0]=edVtHd0;
					orgDiagonal[1]=edVtHd1;

					newDiagonal[0]=quadVtHd[1];
					newDiagonal[1]=quadVtHd[3];

					triPlHd[0]=edPlHd[0];
					triPlHd[1]=edPlHd[1];

					newTriVtHd[0][0]=quadVtHd[1];
					newTriVtHd[0][1]=quadVtHd[2];
					newTriVtHd[0][2]=quadVtHd[3];

					newTriVtHd[1][0]=quadVtHd[3];
					newTriVtHd[1][1]=quadVtHd[0];
					newTriVtHd[1][2]=quadVtHd[1];

					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::MakeInfo(const YsShell &shl,const YsShellPolygonHandle edPlHd[2])
{
	return MakeInfo(shl,edPlHd[0],edPlHd[1]);
}

YSRESULT YsShell_SwapInfo::MakeInfo(const YsShell &shl,const YsShellPolygonHandle edPlHd0,const YsShellPolygonHandle edPlHd1)
{
	if(edPlHd0!=edPlHd1 && 3==shl.GetNumVertexOfPolygon(edPlHd0) && 3==shl.GetNumVertexOfPolygon(edPlHd1))
	{
		YsArray <YsShellVertexHandle,4> plVtHd[2];
		shl.GetVertexListOfPolygon(plVtHd[0],edPlHd0);
		shl.GetVertexListOfPolygon(plVtHd[1],edPlHd1);

		for(int i=0; i<3; ++i)
		{
			for(int j=0; j<3; ++j)
			{
				if(YSTRUE==YsSameEdge(plVtHd[0][i],plVtHd[0].GetCyclic(i+1),plVtHd[1][j],plVtHd[1].GetCyclic(j+1)))
				{
					return MakeInfo(shl,plVtHd[0][i],plVtHd[0].GetCyclic(i+1));
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::ConvexityCheck(const YsShell &shl) const
{
	const YsVec3 quadVtPos[4]=
	{
		shl.GetVertexPosition(quadVtHd[0]),
		shl.GetVertexPosition(quadVtHd[1]),
		shl.GetVertexPosition(quadVtHd[2]),
		shl.GetVertexPosition(quadVtHd[3]),
	};
	const YsVec3 edgeVec[4]=
	{
		quadVtPos[1]-quadVtPos[0],
		quadVtPos[2]-quadVtPos[1],
		quadVtPos[3]-quadVtPos[2],
		quadVtPos[0]-quadVtPos[3],
	};
	const YsVec3 unitEdgeVec[4]=
	{
		YsVec3::UnitVector(edgeVec[0]),
		YsVec3::UnitVector(edgeVec[1]),
		YsVec3::UnitVector(edgeVec[2]),
		YsVec3::UnitVector(edgeVec[3]),
	};
	const YsVec3 crsProd[4]=
	{
		edgeVec[0]^edgeVec[1],
		edgeVec[1]^edgeVec[2],
		edgeVec[2]^edgeVec[3],
		edgeVec[3]^edgeVec[0],
	};
	double dotProd[4]=
	{
		crsProd[0]*crsProd[1],
        crsProd[1]*crsProd[2],
        crsProd[2]*crsProd[3],
		crsProd[3]*crsProd[0],
	};

	if(YSTRUE==YsEqual(1.0,unitEdgeVec[0]*unitEdgeVec[1]))
	{
		dotProd[3]=YsTolerance;
		dotProd[0]=YsTolerance;
	}
	if(YSTRUE==YsEqual(1.0,unitEdgeVec[1]*unitEdgeVec[2]))
	{
		dotProd[0]=YsTolerance;
		dotProd[1]=YsTolerance;
	}
	if(YSTRUE==YsEqual(1.0,unitEdgeVec[2]*unitEdgeVec[3]))
	{
		dotProd[1]=YsTolerance;
		dotProd[2]=YsTolerance;
	}
	if(YSTRUE==YsEqual(1.0,unitEdgeVec[3]*unitEdgeVec[0]))
	{
		dotProd[2]=YsTolerance;
		dotProd[3]=YsTolerance;
	}

	if(0.0<=dotProd[0] &&
	   0.0<=dotProd[1] &&
	   0.0<=dotProd[2] &&
	   0.0<=dotProd[3])
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::SelfIntersectionCheck(const YsShell &shl) const
{
	const YsVec3 dgVtPos[2][2]=
	{
		{shl.GetVertexPosition(orgDiagonal[0]),shl.GetVertexPosition(orgDiagonal[1])},
		{shl.GetVertexPosition(newDiagonal[0]),shl.GetVertexPosition(newDiagonal[1])},
	};
	const YsVec3 dgnVec[2]=
	{
		YsUnitVector(dgVtPos[0][1]-dgVtPos[0][0]),
		YsUnitVector(dgVtPos[1][1]-dgVtPos[1][0])
	};
	YsVec3 nom=dgnVec[0]^dgnVec[1];
	if(YSOK==nom.Normalize())
	{
		YsMatrix3x3 toXY(YSFALSE);
		toXY.MakeToXY(nom);

		const YsVec2 prjQuad[4]=
		{
			(toXY*shl.GetVertexPosition(quadVtHd[0])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[1])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[2])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[3])).xy()
		};
		if(YSTRUE!=YsCheckPolygonSelfIntersection(4,prjQuad))
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::NewDiagonalIsInside(const YsShell &shl) const
{
	const YsVec3 dgVtPos[2][2]=
	{
		{shl.GetVertexPosition(orgDiagonal[0]),shl.GetVertexPosition(orgDiagonal[1])},
		{shl.GetVertexPosition(newDiagonal[0]),shl.GetVertexPosition(newDiagonal[1])},
	};
	const YsVec3 dgnVec[2]=
	{
		YsUnitVector(dgVtPos[0][1]-dgVtPos[0][0]),
		YsUnitVector(dgVtPos[1][1]-dgVtPos[1][0])
	};
	YsVec3 nom=dgnVec[0]^dgnVec[1];
	if(YSOK==nom.Normalize())
	{
		YsMatrix3x3 toXY(YSFALSE);
		toXY.MakeToXY(nom);

		const YsVec2 prjQuad[4]=
		{
			(toXY*shl.GetVertexPosition(quadVtHd[0])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[1])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[2])).xy(),
			(toXY*shl.GetVertexPosition(quadVtHd[3])).xy()
		};
		const YsVec2 midDgn=((toXY*shl.GetVertexPosition(newDiagonal[0])).xy()+(toXY*shl.GetVertexPosition(newDiagonal[1])).xy())/2.0;
		if(YSINSIDE==YsCheckInsidePolygon2(midDgn,4,prjQuad))
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::GeometryCheck(const YsShell &shl) const
{
	const YsVec3 dgVtPos[2][2]=
	{
		{shl.GetVertexPosition(orgDiagonal[0]),shl.GetVertexPosition(orgDiagonal[1])},
		{shl.GetVertexPosition(newDiagonal[0]),shl.GetVertexPosition(newDiagonal[1])},
	};

	YsVec3 n[2];
	if(YSOK==YsGetNearestPointOfTwoLine(n[0],n[1],dgVtPos[0][0],dgVtPos[0][1],dgVtPos[1][0],dgVtPos[1][1]))
	{
		if(YSTRUE==YsCheckInBetween3(n[0],dgVtPos[0]) &&
		   YSTRUE==YsCheckInBetween3(n[1],dgVtPos[1]) &&
		   n[0]!=dgVtPos[0][0] &&
		   n[0]!=dgVtPos[0][1])
		{
			return YSOK;
		}
	}
	else
	{
		// Observed failure case:
		// (-4.307513  -2.496909   1.030980)-(-4.309643  -2.492926   0.009232) : Distances from newDiagonal point to the original diagonal 0.000699 0.001598
		// Distances are much greater than the tolerance.
		// Bad news: Merging bubbles to edge before this point won't help.

		const YsVec3 dgnVec[2]=
		{
			YsUnitVector(dgVtPos[0][1]-dgVtPos[0][0]),
			YsUnitVector(dgVtPos[1][1]-dgVtPos[1][0])
		};
		YsVec3 nom=dgnVec[0]^dgnVec[1];
		if(YSOK==nom.Normalize())
		{
			YsMatrix3x3 toXY(YSFALSE);
			toXY.MakeToXY(nom);

			const YsVec2 prjQuad[4]=
			{
				(toXY*shl.GetVertexPosition(quadVtHd[0])).xy(),
				(toXY*shl.GetVertexPosition(quadVtHd[1])).xy(),
				(toXY*shl.GetVertexPosition(quadVtHd[2])).xy(),
				(toXY*shl.GetVertexPosition(quadVtHd[3])).xy()
			};
			if(YSTRUE==YsCheckConvex2(4,prjQuad))
			{
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell_SwapInfo::NormalImprovementCheck(const YsShell &shl,const YsVec3 vtxNom[4]) const
{
	double errorBefore[2],errorAfter[2];
	CalculateNormalError(shl,errorBefore,errorAfter,vtxNom);

	if(YsSmaller(errorAfter[0],errorAfter[1])>YsSmaller(errorBefore[0],errorBefore[1]))
	{
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShell_SwapInfo::FaceGroupBecomeSmoother(const YsShellExt &shl) const
{
	auto fgHd=shl.FindFaceGroupFromPolygon(triPlHd[0]);

	double oldDhaCos=1.0,newDhaCos=1.0;

	// Old 0-1-2, 2-3-0
	YsShell::VertexHandle oldTri[2][3]=
	{
		{quadVtHd[0],quadVtHd[1],quadVtHd[2]},
		{quadVtHd[2],quadVtHd[3],quadVtHd[0]},
	};

	// New 1-2-3, 3-0-1
	YsShell::VertexHandle newTri[2][3]=
	{
		{quadVtHd[1],quadVtHd[2],quadVtHd[3]},
		{quadVtHd[3],quadVtHd[0],quadVtHd[1]},
	};

	if(CalculateCosineOfMaxDhaWithinSameFaceGroup(shl,oldTri,fgHd)<CalculateCosineOfMaxDhaWithinSameFaceGroup(shl,newTri,fgHd))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

double YsShell_SwapInfo::CalculateCosineOfMaxDhaWithinSameFaceGroup(const YsShellExt &shl,const YsShell::VertexHandle tri[2][3],YsShellExt::FaceGroupHandle fgHd) const
{
	YsVec3 nom[2];
	for(int i=0; i<2; ++i)
	{
		const YsVec3 triVtPos[3]=
		{
			shl.GetVertexPosition(tri[i][0]),
			shl.GetVertexPosition(tri[i][1]),
			shl.GetVertexPosition(tri[i][2])
		};
		nom[i]=YsGetAverageNormalVector(3,triVtPos);
	}
	double cosineOfDha=nom[0]*nom[1];

	for(int i=0; i<2; ++i)
	{
		for(int j=0; j<2; ++j)
		{
			for(auto neiPlHd : shl.FindPolygonFromEdgePiece(tri[i][j],tri[i][j+1]))
			{
				if(neiPlHd!=triPlHd[0] && neiPlHd!=triPlHd[1] && fgHd==shl.FindFaceGroupFromPolygon(neiPlHd))
				{
					auto n=YsShell_CalculateNormal(shl.Conv(),neiPlHd);
					YsMakeSmaller(cosineOfDha,nom[i]*n);
				}
			}
		}
	}
	return cosineOfDha;
}


void YsShell_SwapInfo::CalculateNormalError(const YsShell &shl,double errorBefore[2],double errorAfter[2],const YsVec3 vtxNom[4]) const
{
	for(int triIdx=0; triIdx<2; ++triIdx)
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,triPlHd[triIdx]);
		YsVec3 nom;
		YsGetAverageNormalVector(nom,plVtPos);

		double minCos=1.0;
		for(int vtIdx=0; vtIdx<3; ++vtIdx)
		{
			const double dot=nom*vtxNom[(triIdx*2+vtIdx)%4];
			YsMakeSmaller(minCos,dot);
		}

		errorBefore[triIdx]=minCos;
	}

	for(int triIdx=0; triIdx<2; ++triIdx)
	{
		// {1,2,3} , {3,0,1}
		YsVec3 plVtPos[3]=
		{
			shl.GetVertexPosition(newTriVtHd[triIdx][0]),
			shl.GetVertexPosition(newTriVtHd[triIdx][1]),
			shl.GetVertexPosition(newTriVtHd[triIdx][2])
		};
		YsVec3 nom;
		YsGetAverageNormalVector(nom,3,plVtPos);

		double minCos=1.0;
		for(int vtIdx=0; vtIdx<3; ++vtIdx)
		{
			const double dot=nom*vtxNom[(1+triIdx*2+vtIdx)%4];
			YsMakeSmaller(minCos,dot);
		}

		errorAfter[triIdx]=minCos;
	}
}


////////////////////////////////////////////////////////////

YsShell_MergeInfo::YsShell_MergeInfo()
{
	CleanUp();
}

void YsShell_MergeInfo::CleanUp(void)
{
	plHd[0]=NULL;
	plHd[1]=NULL;
	newPlVtHd.CleanUp();
	duplicateWarning=YSFALSE;
}

YSRESULT YsShell_MergeInfo::MakeInfo(const YsShell &shl,const YsShellPolygonHandle plHd[2])
{
	return MakeInfo(shl,plHd[0],plHd[1]);
}

YSRESULT YsShell_MergeInfo::MakeInfo(const YsShell &shl,const YsShellPolygonHandle plHd0,const YsShellPolygonHandle plHd1)
{
	CleanUp();

	YsArray <YsShellVertexHandle> plVtHd[2];
	shl.GetVertexListOfPolygon(plVtHd[0],plHd0);
	shl.GetVertexListOfPolygon(plVtHd[1],plHd1);

	for(int i=0; i<2; ++i)
	{
		printf("[%d]",i);
		for(int j=0; j<plVtHd[i].GetN(); ++j)
		{
			printf(" %d",shl.GetSearchKey(plVtHd[i][j]));
		}
		printf("\n");
	}

	// Find first shared edge
	YSSIZE_T sharedEdIdx[2]={-1,-1};
	for(YSSIZE_T i=0; i<plVtHd[0].GetN(); ++i)
	{
		for(YSSIZE_T j=0; j<plVtHd[1].GetN(); ++j)
		{
			if(plVtHd[0][i]==plVtHd[1][j] && plVtHd[0].GetCyclic(i+1)==plVtHd[1].GetCyclic(j+1))
			{
				sharedEdIdx[0]=i;
				sharedEdIdx[1]=j;
			}
			else if(plVtHd[0][i]==plVtHd[1].GetCyclic(j+1) && plVtHd[0].GetCyclic(i+1)==plVtHd[1][j])
			{
				sharedEdIdx[0]=i;
				plVtHd[1].Invert();
				sharedEdIdx[1]=plVtHd[1].GetN()-1-(j+1)%plVtHd[1].GetN();
			}
		}
	}

	// Validate
	if(0>sharedEdIdx[0] || 0>sharedEdIdx[1])
	{
		return YSERR;
	}

	if(YSTRUE!=YsSameEdge(plVtHd[0][sharedEdIdx[0]],plVtHd[0].GetCyclic(1+sharedEdIdx[0]),
	                      plVtHd[1][sharedEdIdx[1]],plVtHd[1].GetCyclic(1+sharedEdIdx[1])))
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  Internal error.  Shared edge identification failed.\n");
		return YSERR;
	}

	// Rotate
	{
		YsArray <YsShellVertexHandle,16> A,B;
		if(0!=sharedEdIdx[0])
		{
			A.Set(sharedEdIdx[0],plVtHd[0]);
			B.Set(plVtHd[0].GetN()-sharedEdIdx[0],plVtHd[0]+sharedEdIdx[0]);
			plVtHd[0]=B;
			plVtHd[0].Append(A);
		}

		if(0!=sharedEdIdx[1])
		{
			A.Set(sharedEdIdx[1],plVtHd[1]);
			B.Set(plVtHd[1].GetN()-sharedEdIdx[1],plVtHd[1]+sharedEdIdx[1]);
			plVtHd[1]=B;
			plVtHd[1].Append(A);
		}
	}

	// Trim mid shared sequence
	while(3<=plVtHd[0].GetN() &&
	      3<=plVtHd[1].GetN() &&
	      plVtHd[0][0]==plVtHd[1][0] &&
	      plVtHd[0][1]==plVtHd[1][1] &&
	      plVtHd[0][2]==plVtHd[1][2])
	{
		plVtHd[0].Delete(1);
		plVtHd[1].Delete(1);
	}
	while(3<=plVtHd[0].GetN() &&
	      3<=plVtHd[1].GetN() &&
	      plVtHd[0].GetEnd()==plVtHd[1].GetEnd() &&
	      plVtHd[0][0]==plVtHd[1][0] &&
	      plVtHd[0][1]==plVtHd[1][1])
	{
		YsShellVertexHandle swp=plVtHd[0].GetEnd();
		plVtHd[0][0]=swp;
		plVtHd[0].DeleteLast();

		swp=plVtHd[1].GetEnd();
		plVtHd[1][0]=swp;
		plVtHd[1].DeleteLast();
	}

	for(int i=0; i<2; ++i)
	{
		printf("[%d]",i);
		for(int j=0; j<plVtHd[i].GetN(); ++j)
		{
			printf(" %d",shl.GetSearchKey(plVtHd[i][j]));
		}
		printf("\n");
	}

	// Finally assemble merged polygon
	if(2<=plVtHd[0].GetN() && 
	   2<=plVtHd[1].GetN() &&
	   YSTRUE==YsSameEdge(plVtHd[0][0],plVtHd[0][1],plVtHd[1][0],plVtHd[1][1]))
	{
		YsShellVertexHandle swp=plVtHd[0][0];
		plVtHd[0].Append(swp);
		plVtHd[0].Delete(0);

		swp=plVtHd[1][0];
		plVtHd[1].Append(swp);
		plVtHd[1].Delete(0);

		newPlVtHd=plVtHd[0];
		plVtHd[1].Invert();
		newPlVtHd.Append(plVtHd[1].GetN()-2,plVtHd[1]+1);

		for(int i=0; i<newPlVtHd.GetN(); ++i)
		{
			for(int j=i+1; j<newPlVtHd.GetN(); ++j)
			{
				if(newPlVtHd[i]==newPlVtHd[j])
				{
					duplicateWarning=YSTRUE;
				}
			}
		}

		if(3<=newPlVtHd.GetN())
		{
			return YSOK;
		}
	}
	return YSERR;
}

const YsConstArrayMask <YsShell::VertexHandle> YsShell_MergeInfo::GetNewPolygon(void) const
{
	const YsConstArrayMask <YsShell::VertexHandle> mask(newPlVtHd.GetN(),newPlVtHd);
	return mask;
}

////////////////////////////////////////////////////////////

YsShell_CollapseInfo::YsShell_CollapseInfo()
{
	CleanUp();
}

void YsShell_CollapseInfo::CleanUp(void)
{
	newPlg.Set(0,NULL);
	newConstEdge.Set(0,NULL);
	plHdToDel.Set(0,NULL);
	ceHdToDel.Set(0,NULL);
	vtHdToDel=NULL;
	vtHdToSurvive=NULL;
}

YSRESULT YsShell_CollapseInfo::MakeInfo(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2])
{
	return MakeInfo(shl,edVtHd[0],edVtHd[1]);
}

YSRESULT YsShell_CollapseInfo::MakeInfo(const YsShellExt &shl,const YsShellVertexHandle vtHdToDel,const YsShellVertexHandle vtHdToSurvive)
{
	CleanUp();

	YsArray <YsShellPolygonHandle> vtPlHd;
	if(YSOK==shl.FindPolygonFromVertex(vtPlHd,vtHdToDel))
	{
		for(YSSIZE_T plIdx=0; plIdx<vtPlHd.GetN(); ++plIdx)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetVertexListOfPolygon(plVtHd,vtPlHd[plIdx]);

			for(auto i=plVtHd.GetN()-1; 0<=i; --i)
			{
				if(plVtHd[i]==vtHdToDel)
				{
					plVtHd[i]=vtHdToSurvive;
				}

				if(plVtHd[i]==plVtHd.GetCyclic(i+1))
				{
					plVtHd.Delete(i);
				}
			}

			if(0<plVtHd.GetN() && plVtHd[0]==plVtHd.Last())
			{
				plVtHd.DeleteLast();
			}

			if(3<=plVtHd.GetN())
			{
				newPlg.Increment();
				newPlg.GetEnd().plHd=vtPlHd[plIdx];
				newPlg.GetEnd().plVtHd=plVtHd;
			}
			else
			{
				plHdToDel.Append(vtPlHd[plIdx]);
			}
		}
	}

	YsArray <YsShellExt::ConstEdgeHandle> vtCeHd;
	if(YSOK==shl.FindConstEdgeFromVertex(vtCeHd,vtHdToDel))
	{
		for(YSSIZE_T ceIdx=0; ceIdx<vtCeHd.GetN(); ++ceIdx)
		{
			YsArray <YsShellVertexHandle,4> ceVtHd;
			YSBOOL isLoop;
			shl.GetConstEdge(ceVtHd,isLoop,vtCeHd[ceIdx]);

			if(YSTRUE!=isLoop)
			{
				for(auto i=ceVtHd.GetN()-1; 0<=i; --i)
				{
					if(ceVtHd[i]==vtHdToDel)
					{
						ceVtHd[i]=vtHdToSurvive;
					}

					if(ceVtHd[i]==ceVtHd.GetCyclic(i+1))
					{
						ceVtHd.Delete(i);
					}
				}

				if(2<=ceVtHd.GetN())
				{
					newConstEdge.Increment();
					newConstEdge.GetEnd().ceHd=vtCeHd[ceIdx];
					newConstEdge.GetEnd().ceVtHd=ceVtHd;
					newConstEdge.GetEnd().isLoop=isLoop;
				}
				else
				{
					ceHdToDel.Append(vtCeHd[ceIdx]);
				}
			}
			else
			{
				for(auto i=ceVtHd.GetN()-1; 0<=i; --i)
				{
					if(ceVtHd[i]==vtHdToDel)
					{
						ceVtHd[i]=vtHdToSurvive;
					}

					if(ceVtHd[i]==ceVtHd.GetCyclic(i+1))
					{
						ceVtHd.Delete(i);
					}
				}

				if(3<=ceVtHd.GetN())
				{
					newConstEdge.Increment();
					newConstEdge.GetEnd().ceHd=vtCeHd[ceIdx];
					newConstEdge.GetEnd().ceVtHd=ceVtHd;
					newConstEdge.GetEnd().isLoop=isLoop;
				}
				else if(2==ceVtHd.GetN())
				{
					newConstEdge.Increment();
					newConstEdge.GetEnd().ceHd=vtCeHd[ceIdx];
					newConstEdge.GetEnd().ceVtHd=ceVtHd;
					newConstEdge.GetEnd().isLoop=YSFALSE;
				}
				else
				{
					ceHdToDel.Append(vtCeHd[ceIdx]);
				}
			}
		}
	}

	this->vtHdToSurvive=vtHdToSurvive;
	this->vtHdToDel=vtHdToDel;

	return YSOK;
}

YsShell::Edge YsShell_CollapseInfo::GetTrailingEdge(const YsShellExt &shl,YsShell::PolygonHandle plHdToDel) const
{
	return GetTrailingEdge(shl,plHdToDel,vtHdToDel,vtHdToSurvive);
}
/* static */ YsShell::Edge YsShell_CollapseInfo::GetTrailingEdge(const YsShellExt &shl,YsShell::PolygonHandle plHdToDel,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd)
{
	auto plVtHd=shl.GetPolygonVertex(plHdToDel);
	for(auto idx : plVtHd.AllIndex())
	{
		YsShell::Edge edge(plVtHd[idx],plVtHd.GetCyclic(idx+1));
		if(YSTRUE!=YsSameEdge(edge[0],edge[1],fromVtHd,toVtHd) &&
		   (edge[0]==fromVtHd || edge[1]==fromVtHd))
		{
			return edge;
		}
	}
	return YsShell::Edge(nullptr,nullptr);
}

void YsShell_CollapseInfo::GetVanishingEdge(YsArray <YsShell::Edge,2> &edge,const YsShellExt &shl) const
{
	auto connVtHd1=shl.GetConnectedVertex(vtHdToDel);
	auto connVtHd2=shl.GetConnectedVertex(vtHdToSurvive);

	edge.CleanUp();
	for(auto vtHd1 : connVtHd1)
	{
		for(auto vtHd2 : connVtHd2)
		{
			if(vtHd1==vtHd2)
			{
				edge.Increment();
				edge.Last().Set(vtHdToDel,vtHd1);
				break;
			}
		}
	}
}

YsArray <YsShell::Edge,2> YsShell_CollapseInfo::GetVanishingEdge(const YsShellExt &shl) const
{
	YsArray <YsShell::Edge,2> e;
	GetVanishingEdge(e,shl);
	return e;
}

YsArray <YsShell::Edge> YsShell_CollapseInfo::GetEdgeToDisappear(const YsShellExt &shl) const
{
	YsArray <YsShell::Edge> edgeArray;
	auto connVtHd=shl.GetConnectedVertex(vtHdToDel);
	for(auto vtHd : connVtHd)
	{
		edgeArray.Increment();
		edgeArray.Last()[0]=vtHdToDel;
		edgeArray.Last()[1]=vtHd;
	}
	return edgeArray;
}

YsArray <YsShell::Edge> YsShell_CollapseInfo::GetNewEdge(const YsShellExt &shl) const
{
	YsArray <YsShell::Edge> edgeArray;
	auto connVtHd=shl.GetConnectedVertex(vtHdToDel);
	for(auto vtHd : connVtHd)
	{
		if(vtHdToSurvive!=vtHd && 0==shl.GetNumPolygonUsingEdge(vtHdToSurvive,vtHd)) // If the edges does not exist already.
		{
			edgeArray.Increment();
			edgeArray.Last()[0]=vtHdToSurvive;
			edgeArray.Last()[1]=vtHd;
		}
	}
	return edgeArray;
}

YsShell_CollapseInfo::Log YsShell_CollapseInfo::GetLog(const YsShellExt &shl) const
{
	Log log;
	log.fromVtHd=vtHdToDel;
	log.toVtHd=vtHdToSurvive;

	log.plgBeforeCollapse.Resize(newPlg.GetN());
	for(auto idx : newPlg.AllIndex())
	{
		log.plgBeforeCollapse[idx].hashKey=shl.GetSearchKey(newPlg[idx].plHd);
		log.plgBeforeCollapse[idx].plVtHd=shl.GetPolygonVertex(newPlg[idx].plHd);
	}
	log.deletedPlg.Resize(plHdToDel.GetN());
	for(auto idx : plHdToDel.AllIndex())
	{
		log.deletedPlg[idx].hashKey=shl.GetSearchKey(plHdToDel[idx]);
		log.deletedPlg[idx].plVtHd=shl.GetPolygonVertex(plHdToDel[idx]);
	}

	return log;
}

/*static*/ YsArray <YsShell::PolygonHandle> YsShell_CollapseInfo::GetChangedPolygonHandleFromLog(const YsShellExt &shl,const Log &log)
{
	YsArray <YsShell::PolygonHandle> plHdArray;
	for(auto &modifiedPlg : log.plgBeforeCollapse)
	{
		auto curPlVtHd=modifiedPlg.plVtHd;
		for(auto &vtHd : curPlVtHd)
		{
			if(log.fromVtHd==vtHd)
			{
				vtHd=log.toVtHd;
			}
		}
		auto plHd=shl.FindPolygonFromVertexSequence(curPlVtHd);
		if(nullptr!=plHd)
		{
			plHdArray.Add(plHd);
		}
	}
	return plHdArray;
}

YsShell_CollapseInfo::TopologyChange YsShell_CollapseInfo::CheckTopologyChange(const YsShellExt &shl) const
{
	TopologyChange chg;
	for(auto connVtHd : shl.GetConnectedVertex(vtHdToDel))
	{
		// nValenceBefore is simplyl the number of polygons using connVtHd-vtHdToDel.
		// nValenceAfter is sum of the number of polygons using connVtHd-vtHdToDel and connVtHd-vtHdToSurvive
		// excluding the disappearing polygons.
		int nValenceBefore=0,nValenceAfter=0;
		for(auto connPlHd : shl.FindPolygonFromEdgePiece(connVtHd,vtHdToDel))
		{
			++nValenceBefore;
			if(YSTRUE!=plHdToDel.IsIncluded(connPlHd))
			{
				++nValenceAfter;
			}
		}
		for(auto connPlHd : shl.FindPolygonFromEdgePiece(connVtHd,vtHdToSurvive))
		{
			if(YSTRUE!=plHdToDel.IsIncluded(connPlHd))
			{
				++nValenceAfter;
			}
		}

		if(nValenceBefore!=nValenceAfter)
		{
			chg.valenceChange=YSTRUE;
		}
		if(nValenceBefore<=2 && 2<nValenceAfter)
		{
			chg.newOverUsedEdge=YSTRUE;
		}
		if(1==nValenceBefore && 2<=nValenceAfter)
		{
			chg.singleUseEdgeClose=YSTRUE;
		}
	}
	return chg;
}

////////////////////////////////////////////////////////////

YSRESULT YsShellExt_EdgeVertexInsertionInfo::MakeInfo(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2],YSSIZE_T nVtInsert,const YsShellVertexHandle vtInsert[])
{
	return MakeInfo(shl,edVtHd[0],edVtHd[1],nVtInsert,vtInsert);
}

YSRESULT YsShellExt_EdgeVertexInsertionInfo::MakeInfo(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YSSIZE_T nVtInsert,const YsShellVertexHandle vtInsert[])
{
	plChange.CleanUp();

	auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd0,edVtHd1);
	for(auto plHd : edPlHd)
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);

		for(auto idx : plVtHd.AllIndex())
		{
			if(plVtHd[idx]==edVtHd0 && plVtHd.GetCyclic(idx+1)==edVtHd1)
			{
				for(YSSIZE_T j=0; j<nVtInsert; ++j)
				{
					plVtHd.Insert(idx+1+j,vtInsert[j]);
				}
				break;
			}
			else if(plVtHd[idx]==edVtHd1 && plVtHd.GetCyclic(idx+1)==edVtHd0)
			{
				for(YSSIZE_T j=0; j<nVtInsert; ++j)
				{
					plVtHd.Insert(idx+1+j,vtInsert[nVtInsert-1-j]);
				}
				break;
			}
		}

		plChange.Increment();
		plChange.Last().plHd=plHd;
		plChange.Last().plVtHd.MoveFrom(plVtHd);
	}

	auto edCeHd=shl.FindConstEdgeFromEdgePiece(edVtHd0,edVtHd1);
	for(auto ceHd : edCeHd)
	{
		YSBOOL isLoop;
		YsArray <YsShellVertexHandle,16> ceVtHd;
		shl.GetConstEdge(ceVtHd,isLoop,ceHd);

		const YSSIZE_T idx1=(YSTRUE!=isLoop ? ceVtHd.GetN()-1 : ceVtHd.GetN());
		for(YSSIZE_T idx=0; idx<idx1; ++idx)
		{
			if(ceVtHd[idx]==edVtHd0 && ceVtHd.GetCyclic(idx+1)==edVtHd1)
			{
				for(YSSIZE_T j=0; j<nVtInsert; ++j)
				{
					ceVtHd.Insert(idx+1+j,vtInsert[j]);
				}
				break;
			}
			else if(ceVtHd[idx]==edVtHd1 && ceVtHd.GetCyclic(idx+1)==edVtHd0)
			{
				for(YSSIZE_T j=0; j<nVtInsert; ++j)
				{
					ceVtHd.Insert(idx+1+j,vtInsert[nVtInsert-1-j]);
				}
				break;
			}
		}

		ceChange.Increment();
		ceChange.Last().ceHd=ceHd;
		ceChange.Last().ceVtHd.MoveFrom(ceVtHd);
		ceChange.Last().isLoop=isLoop;
	}

	return YSOK;
}

const YsArray <YsShell_LocalOperation::PolygonChange> &YsShellExt_EdgeVertexInsertionInfo::PolygonChange(void) const
{
	return plChange;
}

const YsArray <YsShell_LocalOperation::ConstEdgeChange> &YsShellExt_EdgeVertexInsertionInfo::ConstEdgeChange(void) const
{
	return ceChange;
}

////////////////////////////////////////////////////////////

YsArray <YsShellVertexHandle> YsShellExt_TriangulatePolygon(const YsShell &shl,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd)
{
	YsArray <YsShellVertexHandle> triVtHd;

	YsArray <YsVec3,4> plVtPos;
	YsArray <int,4> plVtIdx;

	plVtPos.Set(plVtHd.GetN(),NULL);
	plVtIdx.Set(plVtHd.GetN(),NULL);
	for(auto idx : plVtHd.AllIndex())
	{
		shl.GetVertexPosition(plVtPos[idx],plVtHd[idx]);
		plVtIdx[idx]=(int)idx;
	}

	// First try Delaunay
	{
		YSRESULT res;
		auto triIdx=YsTriangulatePolygonDelaunay(res,plVtPos);
		if(YSOK==res)
		{
			triVtHd.Resize(triIdx.size());
			for(YSSIZE_T idx=0; idx<triIdx.size(); ++idx)
			{
				triVtHd[idx]=plVtHd[triIdx[idx]];
			}
			return triVtHd;
		}
	}

	// Then see if it is convex.
	if(YSTRUE==YsCheckConvexByAngle3(plVtPos))
	{
		auto triIdx=YsTriangulateConvexPolygon(plVtPos);
		triVtHd.Resize(triIdx.size());
		for(YSSIZE_T idx=0; idx<triIdx.size(); ++idx)
		{
			triVtHd[idx]=plVtHd[triIdx[idx]];
		}
		return triVtHd;
	}

	// Last resort.  Use YsSword.
	YsSword sword;
	sword.SetInitialPolygon(plVtPos.GetN(),plVtPos,plVtIdx);
	if(YSOK==sword.Triangulate())
	{
		triVtHd.Resize(sword.GetNumPolygon()*3);

		for(int triIdx=0; triIdx<sword.GetNumPolygon(); ++triIdx)
		{
			const YsArray <int> *tri=sword.GetVertexIdList(triIdx);
			if(NULL!=tri && 3==tri->GetN())
			{
				triVtHd[triIdx*3  ]=plVtHd[plVtIdx[(*tri)[0]]];
				triVtHd[triIdx*3+1]=plVtHd[plVtIdx[(*tri)[1]]];
				triVtHd[triIdx*3+2]=plVtHd[plVtIdx[(*tri)[2]]];
			}
			else
			{
				triVtHd.CleanUp();
				break;
			}
		}
	}

	return triVtHd;
}

YsArray <YsShellVertexHandle> YsShellExt_TriangulatePolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	shl.GetPolygon(plVtHd,plHd);

	return YsShellExt_TriangulatePolygon(shl,plVtHd);
}


