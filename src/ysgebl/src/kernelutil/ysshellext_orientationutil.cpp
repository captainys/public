/* ////////////////////////////////////////////////////////////

File Name: ysshellext_orientationutil.cpp
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

#include "ysshellext_orientationutil.h"



YsShellExt_OrientationUtil::YsShellExt_OrientationUtil()
{
}

YsShellExt_OrientationUtil::~YsShellExt_OrientationUtil()
{
}

void YsShellExt_OrientationUtil::CleanUp(void)
{
	visitedPlHd.CleanUp();
	plHdNeedFlipArray.CleanUp();
	plHdNeedNormalChangeArray.CleanUp();
	abortFlag=YSFALSE;
	fixOrientetionFromReliablePolygonEnded=YSFALSE;
	fixOrientationOfClosedSolidEnded=YSFALSE;
	normalRecalculationEnded=YSFALSE;
}

YSRESULT YsShellExt_OrientationUtil::FixOrientationFromReliablePolygon(const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YSBOOL seedPlgNeedFlip)
{
	YSSIZE_T n;
	return FixOrientationFromReliablePolygon(n,shl,seedPlHd,seedPlgNeedFlip);
}

YSRESULT YsShellExt_OrientationUtil::FixOrientationFromReliablePolygon(YSSIZE_T &nPlgCovered,const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YSBOOL seedPlgNeedFlip)
{
	nPlgCovered=0;
	visitedPlHd.SetShell((const YsShell &)shl);

	if(YSTRUE==visitedPlHd.IsIncluded(seedPlHd))
	{
		fixOrientetionFromReliablePolygonEnded=YSTRUE;
		return YSERR;
	}

	this->shl=&shl;
	visitedPlHd.AddPolygon(seedPlHd);

	YsArray <YsShellPolygonHandle> todo;
	YsArray <YSBOOL> todoNeedFlip;
	todo.Append(seedPlHd);
	todoNeedFlip.Append(seedPlgNeedFlip);

	if(YSTRUE==seedPlgNeedFlip)
	{
		plHdNeedFlipArray.Append(seedPlHd);
	}

	while(0<todo.GetN() && YSTRUE!=abortFlag)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,todo[0]);
		++nPlgCovered;

		for(int vtIdx=0; vtIdx<plVtHd.GetN() && YSTRUE!=abortFlag; ++vtIdx)
		{
			YsShellPolygonHandle neiPlHd=shl.GetNeighborPolygon(todo[0],vtIdx);
			if(NULL!=neiPlHd && YSTRUE!=visitedPlHd.IsIncluded(neiPlHd))
			{
				visitedPlHd.AddPolygon(neiPlHd);
				todo.Append(neiPlHd);
				todoNeedFlip.Append(YSFALSE); // Tentative

				const YsShellVertexHandle edVtHd[2]={plVtHd[vtIdx],plVtHd.GetCyclic(vtIdx+1)};
				YsArray <YsShellVertexHandle,4> neiPlVtHd;

				shl.GetPolygon(neiPlVtHd,neiPlHd);

				for(int neiVtIdx=0; neiVtIdx<neiPlVtHd.GetN() && YSTRUE!=abortFlag; ++neiVtIdx)
				{
					if(edVtHd[0]==neiPlVtHd[neiVtIdx] && edVtHd[1]==neiPlVtHd.GetCyclic(neiVtIdx+1))
					{
						if(YSTRUE!=todoNeedFlip[0])
						{
							plHdNeedFlipArray.Append(neiPlHd);
							todoNeedFlip.Last()=YSTRUE;
						}
						break;
					}
					else if(edVtHd[1]==neiPlVtHd[neiVtIdx] && edVtHd[0]==neiPlVtHd.GetCyclic(neiVtIdx+1))
					{
						if(YSTRUE==todoNeedFlip[0])
						{
							plHdNeedFlipArray.Append(neiPlHd);
							todoNeedFlip.Last()=YSTRUE;
						}
						break;
					}
				}
			}
		}

		todo.DeleteBySwapping(0);
		todoNeedFlip.DeleteBySwapping(0);
	}

	fixOrientetionFromReliablePolygonEnded=YSTRUE;
	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::FixOrientationBasedOnAssignedNormal(const YsShellExt &shl)
{
	plHdNeedFlipArray.CleanUp();
	for(auto plHd : shl.AllPolygon())
	{
		YsVec3 refNom=shl.GetNormal(plHd);
		if(YsOrigin()!=refNom)
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,plHd);

			YsVec3 nom;
			YsGetAverageNormalVector(nom,plVtPos);
			if(nom*refNom<0.0)
			{
				plHdNeedFlipArray.Append(plHd);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::FixOrientationOfClosedSolid(const YsShellExt &shl)
{
	visitedPlHd.SetShell((const YsShell &)shl);

	YsShellLattice ltc;

	const int nLtcCell=1000+shl.GetNumPolygon(); // *100 -> Given 0.3G polygon model...

	ltc.SetDomain((const YsShell &)shl,nLtcCell);
	ltc.MakeSubLattice(nLtcCell);

	for(auto plHd : shl.AllPolygon())
	{
		if(YSTRUE==abortFlag)
		{
			break;
		}

		if(YSTRUE!=visitedPlHd.IsIncluded(plHd))
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,plHd);

			YsVec3 nom;
			if(YSOK==YsGetAverageNormalVector(nom,plVtPos))
			{
				YsVec3 cen;
				shl.GetCenterOfPolygon(cen,plHd);

				YsShellPolygonHandle itscPlHd;
				YsVec3 itscPos;

				if(YSOK==ltc.FindFirstIntersection(itscPlHd,itscPos,cen,nom,plHd))
				{
					if(NULL!=itscPlHd && cen!=itscPos)
					{
						const YSSIDE side=ltc.CheckInsideSolid((itscPos+cen)/2.0);
						if(YSINSIDE==side)
						{
							// This is an incorrectly-oriented polygon!
							FixOrientationFromReliablePolygon(shl,plHd,/*seedPlgNeedFlip=*/YSTRUE);
						}
						else if(YSOUTSIDE==side)
						{
							// This is a correctly-oriented polygon!
							FixOrientationFromReliablePolygon(shl,plHd,/*seedPlgNeedFlip=*/YSFALSE);
						}
					}
					else if(NULL==itscPlHd)
					{
						// This is a correctly-oriented polygon!
						FixOrientationFromReliablePolygon(shl,plHd,/*seedPlgNeedFlip=*/YSFALSE);
					}
				}
			}
		}
	}

	fixOrientationOfClosedSolidEnded=YSTRUE;

	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::RecalculateNormalFromCurrentOrientation(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);

	YsVec3 newNom;
	if(YSOK==YsGetAverageNormalVector(newNom,plVtPos))
	{
		YsVec3 curNom;
		shl.GetNormal(curNom,plHd);

		if(newNom!=curNom)
		{
			plHdNeedNormalChangeArray.Increment();
			plHdNeedNormalChangeArray.Last().plHd=plHd;
			plHdNeedNormalChangeArray.Last().nom=newNom;
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt_OrientationUtil::RecalculateNormalFromCurrentOrientation(const YsShell &shl)
{
	for(auto plHd : shl.AllPolygon())
	{
		if(YSTRUE==abortFlag)
		{
			break;
		}
		RecalculateNormalFromCurrentOrientation(shl,plHd);
	}
	normalRecalculationEnded=YSTRUE;
	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::RecalculateNormalFromCurrentOrientation(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		if(YSTRUE==abortFlag)
		{
			break;
		}
		RecalculateNormalFromCurrentOrientation(shl,plHdArray[idx]);
	}
	normalRecalculationEnded=YSTRUE;
	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::RecalculateNormalRespectingCurrentNormal(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);

	const YsVec3 curNom=shl.GetNormal(plHd);

	YsVec3 newNom=YsGetAverageNormalVector(plVtPos);
	if(newNom*curNom<0.0)
	{
		newNom=-newNom;
		plHdNeedFlipArray.Append(plHd);
	}

	if(newNom!=curNom)
	{
		plHdNeedNormalChangeArray.Increment();
		plHdNeedNormalChangeArray.Last().plHd=plHd;
		plHdNeedNormalChangeArray.Last().nom=newNom;
	}

	return YSOK;
}

YSRESULT YsShellExt_OrientationUtil::RecalculateNormalRespectingCurrentNormal(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		RecalculateNormalRespectingCurrentNormal(shl,plHdArray[idx]);
	}
	return YSOK;
}

const YsArray <YsShellPolygonHandle> &YsShellExt_OrientationUtil::GetPolygonNeedFlip(void) const
{
	return plHdNeedFlipArray;
}

const YsArray <YsShellExt_OrientationUtil::PolygonNormalPair> &YsShellExt_OrientationUtil::GetPolygonNormalPair(void) const
{
	return plHdNeedNormalChangeArray;
}

/*static*/ void YsShellExt_OrientationUtil::InvertPolygonAll(YsShellExt &shl)
{
	for(auto plHd : shl.AllPolygon())
	{
		YsVec3 nom=shl.GetNormal(plHd);
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,plHd);

		nom=-nom;
		plVtHd.Invert();

		shl.SetPolygonVertex(plHd,plVtHd);
		shl.SetPolygonNormal(plHd,nom);
	}
}

YSRESULT YsShellExt_OrientationUtil::ExpandNormal(const YsShellExt &shl)
{
	plHdNeedNormalChangeArray.CleanUp();

	YsShellPolygonAttribTable <YsVec3> plNom(shl.Conv());
	YsArray <YsShell::PolygonHandle> todo;
	for(auto plHd : shl.AllPolygon())
	{
		auto nom=shl.GetNormal(plHd);
		if(YsVec3::Origin()==nom)
		{
			for(int i=0; i<shl.GetPolygonNumVertex(plHd); ++i)
			{
				auto neiPlHd=shl.GetNeighborPolygon(plHd,i);
				if(nullptr!=neiPlHd &&
				   YsVec3::Origin()!=shl.GetNormal(neiPlHd))
				{
					todo.push_back(plHd);
					break;
				}
			}
		}
		else
		{
			plNom.SetAttrib(plHd,nom);
		}
	}

	for(YSSIZE_T idx=0; idx<todo.size(); ++idx)
	{
		auto plHd=todo[idx];
		if(nullptr==plNom[plHd])
		{
			YsVec3 nomSum=YsVec3::Origin();
			for(int i=0; i<shl.GetPolygonNumVertex(plHd); ++i)
			{
				auto neiPlHd=shl.GetNeighborPolygon(plHd,i);
				if(nullptr!=neiPlHd)
				{
					auto nomPtr=plNom[neiPlHd];
					if(nullptr!=nomPtr)
					{
						nomSum+=*nomPtr;
					}
				}
			}
			if(YSOK==nomSum.Normalize())
			{
				plHdNeedNormalChangeArray.Increment();
				plHdNeedNormalChangeArray.back().plHd=plHd;
				plHdNeedNormalChangeArray.back().nom=nomSum;
				plNom.SetAttrib(plHd,nomSum);

				for(int i=0; i<shl.GetPolygonNumVertex(plHd); ++i)
				{
					auto neiPlHd=shl.GetNeighborPolygon(plHd,i);
					if(nullptr!=neiPlHd &&
					   nullptr==plNom[neiPlHd] &&
					   YsVec3::Origin()==shl.GetNormal(neiPlHd))
					{
						todo.push_back(neiPlHd);
					}
				}
			}
		}
	}

	return YSOK;
}
