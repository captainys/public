/* ////////////////////////////////////////////////////////////

File Name: ysshellblend2.cpp
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

#include "ysclass.h"


// freeAttribute2
//   0:  From shell 0
//   1:  From shell 1
//   2:  Overlapping polygon from shell 0



static YSRESULT YsBlendShell2EdgeFaceIntersection
    (int &nNewVtx,YsShell &newShell,int cutterShlId,YsShellLattice &ltc,const double &cosThr);
static YSRESULT YsShellBlend2InsertVertexToPolygon
    (YsArray <YsShellPolygonHandle,16> &createdPlHd,
     YsShell &newShell,YsShellPolygonHandle cutterPlHd,
     YsArray <YSHASHKEY> &cutterPlKeyList,YsShellVertexHandle vtHd,
     YsShellLattice &ltc,YsArray <YsVec2,16> &plVtPrj,const YsVec2 &p2);
static YSRESULT YsShellBlend2ReconnectEdgeToEliminateSharpCorner
    (YsShell &newshell,	YsArray <YsShellPolygonHandle,16> &createdPlHd,YsShellLattice &ltc,const double &cosThr);
static YSRESULT YsShellBlend2ReconnectEdgeToEliminateSharpCorner
    (YsShell &newShell,YsShellPolygonHandle plHd,YsShellLattice &ltc,const double &cosThr);
static YSRESULT YsShellBlend2EliminateSharpCornerByReconnectingEdge
     (YsShell &newShell,YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,
      const double &refCos,const double &cosThr);
static YSRESULT YsShellBlend2InsertVertexToEdge
	(YsShell &newShell,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2,YsShellVertexHandle newVtHd,
	 YsShellLattice &ltc);
static YSRESULT YsBlendShell2EdgeEdgeIntersection(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp);
static YSRESULT YsBlendShell2EdgeEdgeIntersectionForOneEdge(
    YsShell &newShell,YsShellLattice &ltc,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2,int edgeShlId,YSBOOL singleShellOp);
static YSRESULT YsShellBlend2CheckEdgeVertexInsertionCheck(
    YsShell &newShell,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YsShellVertexHandle insVtHd);
static YSRESULT YsShellBlend2SubstVtHd(YsShell &newShell,YsShellVertexHandle changeThis,YsShellVertexHandle intoThis);
static YSBOOL YsShellBlend2CheckReach(const YsVec2 &pvt,YSSIZE_T np,const YsVec2 p[],int idx);

static YSRESULT YsBlendShell2SlashFaceByFace(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp);
static YSRESULT YsShellBlend2SlashPolygonSet
    (YsArray <YsArray <YsShellVertexHandle,16> > &plVtHdList,
     YsArray <YsArray <YsVec2,16> > &plVtPosList,
     int nCutPlVtHd,const YsShellVertexHandle cutPlVtHd[],const YsVec2 cutPlVtPos[],YSBOOL watch);

static YSRESULT YsBlendShell2OverlappingPolygon(YsShell &newShell,YsShellLattice &ltc,const double &cosThr,YSBOOL singleShellOp);
static YSRESULT YsBlendShell2GetNormalOfPolygon(YsVec3 &nom,YsShell &shl,YsShellPolygonHandle plHd);
static YSRESULT YsBlendShell2GetVertexListOfPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec3,16> &plVtPos,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &prjMat);
static YSRESULT YsBlendShell2GetProjectedPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec3,16> &plVtPos,
	 YsArray <YsVec2,16> &plVtPos2,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &prjMat);
static YSRESULT YsBlendShell2GetProjectedPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec2,16> &plVtPos2,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &prjMat);

static YSBOOL YsBlendShell2CheckAndKillCLoop
    (YsArray <YsShellPolygonHandle,16> &createdPlHd,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsShellPolygonHandle pLoopPlHd,
     const YsMatrix4x4 &prjMat,YSBOOL singleShellOp);

static YSRESULT YsBlendShell2BlendOverlap
    (YsArray <YsShellPolygonHandle,16> &toBeDeleted,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &samePlnPlHd,
     const YsMatrix4x4 &prjMat);
static YSRESULT YsBlendShell2BlendOverlapAfterStitch
    (YsArray <YsShellPolygonHandle,16> &toBeDeleted,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &samePlnPlHd,
     const YsMatrix4x4 &prjMat);
static YSRESULT YsBlendShell2RemoveOverlapBasedOnTopology(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp);
static YSRESULT YsBlendShell2FindOverlap
    (YsArray <YsShellPolygonHandle,16> &samePlnPlHd,YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,const double &cosThr,YSBOOL singleShellOp);

static YSRESULT YsBlendShell2StitchNodeOfOverlapPolygon
    (YsShell &newShell,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsMatrix4x4 &prjMat,
     YsShellLattice &ltc);
static YSRESULT YsBlendShell2StitchEdgeOfOverlapPolygon
    (YsShell &newShell,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsMatrix4x4 &prjMat,
     YsShellLattice &ltc);
static YSRESULT YsShellBlend2SubdivOverlappingPolygon
    (YsArray <YsArray <YsShellVertexHandle,16> > &newPlVtHdList,
     YsArray <YsArray <YsVec2,16> > &newPlVtPosList,
     YsShell &newShell,
     const YsArray <YsShellVertexHandle,16> &plVtHd,
     const YsArray <YsVec2,16> &plVtPos,
     const YsArray <YsShellVertexHandle,16> &cutterVtHd,
     const YsArray <YsVec2,16> &cutterVtPos,
     YSBOOL watch);
// static YSRESULT YsShellBlend2OutputOverlapInspection
// 	(const char fn[],YsShell &shl,YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &plHdList);


// Near garbage.  Not used any more.
// extern YSRESULT YsSlashNewShellByOldShell
//     (YsShell &newShell,int targetShlId,const YsShell &slasher,YSBOOL takeOverlap);

// extern YSRESULT YsBlendAnd(YsShell &newShell,const YsShell &sh1,const YsShell &sh2);
// extern YSRESULT YsBlendOr(YsShell &newShell,const YsShell &sh1,const YsShell &sh2);
// extern YSRESULT YsBlendMinus(YsShell &newShell,const YsShell &sh1,const YsShell &sh2);



////////////////////////////////////////////////////////////


// #define YSDEBUG_STEP_BY_STEP_OUTPUT
// #define YSDEBUG_EDGE_FACE_INTERSECTION
// #define YSDEBUG_EDGE_EDGE_INTERSECTION


#if defined(YSDEBUG_EDGE_EDGE_INTERSECTION) || defined(YSDEBUG_EDGE_FACE_INTERSECTION)
static const YsVec3 watchItscPos[]=
{
	YsVec3(-12.931280,  -0.375277,  -5.703106),
	YsVec3(-12.931285,  -0.375258,  -5.703136),
	YsVec3(-12.931282,  -0.375265,  -5.703125)
};
#endif

static YSBOOL YsSharedPolygonIsBoundary2
    (YsShell &newShell,int sharedPlId,
     const YsShell &sh1,const YsShellLattice &ltc1,int fromPlId1,
     const YsShell &sh2,const YsShellLattice &ltc2,int fromPlId2)
{
	int nVtxOfPlg;
	YsOneMemoryBlock <YsVec3,256> mem;
	YsVec3 *vtx,nom1,nom2;
	YsVec3 lat,lng,inside,ray;
	YSSIDE side1,side2;

	double factor[9][2]=
	{
		{ 0.0, 0.0},
		{ 1.0, 0.0},
		{ 1.0, 1.0},
		{ 0.0, 1.0},
		{-1.0, 1.0},
		{-1.0, 0.0},
		{-1.0,-1.0},
		{ 0.0,-1.0},
		{ 1.0,-1.0}
	};

	nVtxOfPlg=newShell.GetNumVertexOfPolygon(sharedPlId);
	vtx=mem.GetMemoryBlock(nVtxOfPlg);

	if(vtx!=NULL)
	{
		int k;

		newShell.GetVertexListOfPolygon(vtx,nVtxOfPlg,sharedPlId);
		YsGetAverageNormalVector(nom1,nVtxOfPlg,vtx);
		nom1.Normalize();
		nom2=-1.0*nom1;

		YsGetArbitraryInsidePointOfPolygon3(inside,nVtxOfPlg,vtx);

		lat=nom1.GetArbitraryPerpendicularVector();
		lng=nom2^lat;

		lat.Normalize();
		lng.Normalize();

		for(k=0; k<9; k++)
		{
			ray=nom1+lat*factor[k][0]+lng*factor[k][1];

			YsShellPolygonHandle exPlHd1,exPlHd2,plHd1,plHd2;
			exPlHd1=sh1.GetPolygonHandleFromId(fromPlId1);
			exPlHd2=sh2.GetPolygonHandleFromId(fromPlId2);

			YsVec3 itsc1,itsc2,tst;
			if(ltc1.FindFirstIntersection(plHd1,itsc1,inside,ray,exPlHd1)==YSOK &&
			   ltc2.FindFirstIntersection(plHd2,itsc2,inside,ray,exPlHd2)==YSOK)
			{
				if(plHd1!=NULL && plHd2!=NULL)
				{
					if((itsc1-inside).GetSquareLength()<(itsc2-inside).GetSquareLength())
					{
						tst=(itsc1+inside)/2.0;
					}
					else
					{
						tst=(itsc2+inside)/2.0;
					}
				}
				else if(plHd1!=NULL)
				{
					tst=(itsc1+inside)/2.0;
				}
				else if(plHd2!=NULL)
				{
					tst=(itsc2+inside)/2.0;
				}
				else
				{
					// No intersection : Ray is pointing outside of sh1 and sh2
					return YSTRUE;
				}

				side1=ltc1.CheckInsideSolid(tst);
				side2=ltc2.CheckInsideSolid(tst);
				if(side1!=YSBOUNDARY && side1!=YSUNKNOWNSIDE &&
				   side2!=YSBOUNDARY && side2!=YSUNKNOWNSIDE)
				{
					if(side1==side2)
					{
						return YSTRUE;
					}
					else
					{
						return YSFALSE;
					}
				}
			}
		}
	}

	YsErrOut
	    ("YsPolygonIsInside()@ysshell.cpp\n"
	     "  Overlapping polygon may exists.\n");
	return YSTRUE;
}

YSRESULT YsBlendAnd2
    (YsShell &newShell,const YsShell &sh1,const YsShellLattice &lat1,const YsShell &sh2,const YsShellLattice &lat2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	YsPrintf("Entering YsBlendAnd2\n");

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);

		if((plg->freeAttribute2==0 && lat2.CheckInsideSolid(cen)!=YSINSIDE)||
		   (plg->freeAttribute2==1 && lat1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary2
			       (newShell,i,
			        sh1,lat1,plg->freeAttribute3,
			        sh2,lat2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Exiting YsBlendAnd2\n");

	return YSOK;
}

YSRESULT YsBlendOr2
    (YsShell &newShell,const YsShell &sh1,const YsShellLattice &lat1,const YsShell &sh2,const YsShellLattice &lat2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	YsPrintf("Entering YsBlendOr2\n");

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && lat2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && lat1.CheckInsideSolid(cen)==YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary2
			       (newShell,i,
			        sh1,lat1,plg->freeAttribute3,
			        sh2,lat2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Exiting YsBlendOr2\n");

	return YSOK;
}

YSRESULT YsBlendMinus2
    (YsShell &newShell,const YsShell &sh1,const YsShellLattice &lat1,const YsShell &sh2,const YsShellLattice &lat2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;
	YsLoopCounter ctr;

	YsPrintf("Entering YsBlendMinus2\n");

	newShell.Encache();

	ctr.BeginCounter(newShell.GetNumPolygon());

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		ctr.Increment();

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && lat2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && lat1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary2
			       (newShell,i,
			        sh1,lat1,plg->freeAttribute3,
			        sh2,lat2,plg->freeAttribute4)==YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
		else if(plg->freeAttribute2==1)
		{
			newShell.InvertPolygon(i);
		}
	}

	ctr.EndCounter();

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Exiting YsBlendMinus2\n");

	return YSOK;
}

YSRESULT YsShellBoolOpenMinusSolid(YsShell &newShell,const YsShell &,const YsShellLattice &lat2)
{
	// To do:
	// Delete all polygons from shl2
	// Delete all polygons inside shl2

	YsArray <YsVec3,4> vtx;
	YsVec3 cen;
	YsLoopCounter ctr;

	YsPrintf("Entering YsShellBoolOpenMinusSolid\n");

	newShell.Encache();

	ctr.BeginCounter(newShell.GetNumPolygon());

	for(auto plHd : newShell.AllPolygon())
	{
		ctr.Increment();

		auto plg=newShell.GetPolygon(plHd);
		if(1==plg->freeAttribute2)
		{
			newShell.DeletePolygon(plHd);
		}
		else
		{
			newShell.GetPolygon(vtx,plHd);

			YsGetArbitraryInsidePointOfPolygon3(cen,vtx);
			if(lat2.CheckInsideSolid(cen)!=YSOUTSIDE)
			{
				newShell.DeletePolygon(plHd);
			}
		}
	}

	ctr.EndCounter();

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Leaving YsShellBoolOpenMinusSolid\n");

	return YSOK;
}

YSRESULT YsShellBoolOpenPlusSolid(YsShell &newShell,const YsShell &,const YsShellLattice &lat2)
{
	// To do:
	// Delete all polygons inside shl2

	YsArray <YsVec3,4> vtx;
	YsVec3 cen;
	YsLoopCounter ctr;

	YsPrintf("Entering YsShellBoolOpenPlusSolid\n");

	newShell.Encache();

	ctr.BeginCounter(newShell.GetNumPolygon());

	for(auto plHd : newShell.AllPolygon())
	{
		ctr.Increment();

		auto plg=newShell.GetPolygon(plHd);
		if(1!=plg->freeAttribute2)
		{
			newShell.GetPolygon(vtx,plHd);

			YsGetArbitraryInsidePointOfPolygon3(cen,vtx);
			if(lat2.CheckInsideSolid(cen)!=YSOUTSIDE)
			{
				newShell.DeletePolygon(plHd);
			}
		}
	}

	ctr.EndCounter();

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Leaving YsShellBoolOpenPlusSolid\n");

	return YSOK;
}


YSRESULT YsShellBoolOpenTimesSolid(YsShell &newShell,const YsShell &,const YsShellLattice &lat2)
{
	// To do:
	// Delete all polygons from shl2
	// Delete all polygons inside shl2

	YsArray <YsVec3,4> vtx;
	YsVec3 cen;
	YsLoopCounter ctr;

	YsPrintf("Entering YsShellBoolOpenTimesSolid\n");

	newShell.Encache();

	ctr.BeginCounter(newShell.GetNumPolygon());

	for(auto plHd : newShell.AllPolygon())
	{
		ctr.Increment();

		auto plg=newShell.GetPolygon(plHd);
		if(1==plg->freeAttribute2)
		{
			newShell.DeletePolygon(plHd);
		}
		else
		{
			newShell.GetPolygon(vtx,plHd);

			YsGetArbitraryInsidePointOfPolygon3(cen,vtx);
			if(lat2.CheckInsideSolid(cen)!=YSINSIDE)
			{
				newShell.DeletePolygon(plHd);
			}
		}
	}

	ctr.EndCounter();

	int nDel;
	newShell.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsPrintf("Leaving YsShellBoolOpenTimesSolid\n");

	return YSOK;
}


////////////////////////////////////////////////////////////

static void YsBlendShell_AttemptToRecoverFailedEdge(YsShell &shl)
{
// A polygon edge may have failed to be constructed.  However, another polygon may be sharing an edge, 
// and succeessfully constructed.  This function searches for such edges, and re-construct a missing one.

	const YsShellSearchTable &search=*(shl.GetSearchTable());

	YsPrintf("Trying to recover failed edges.\n");

	YsLoopCounter ctr;
	ctr.BeginCounter(shl.GetNumPolygon());

	YsShellPolygonHandle plHd=NULL;
	while(NULL!=(plHd=shl.FindNextPolygon(plHd)))
	{
		ctr.Increment();

	RETRY:
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetVertexListOfPolygon(plVtHd,plHd);

		if(3>plVtHd.GetN())
		{
			continue;
		}

		for(YSSIZE_T fromVtIdx=0; fromVtIdx<plVtHd.GetN(); ++fromVtIdx)
		{
			const auto fromVtHd=plVtHd[fromVtIdx];

			YsArray <YsShellVertexHandle,16> connVtHd;
			search.GetConnectedVertexList(connVtHd,shl,fromVtHd);

			for(YSSIZE_T connVtIdx=0; connVtIdx<connVtHd.GetN(); ++connVtIdx)
			{
				const auto toVtHd=connVtHd[connVtIdx];

				if(toVtHd==plVtHd.GetCyclic(fromVtIdx+1) || toVtHd==plVtHd.GetCyclic(fromVtIdx-1))
				{
					continue;
				}
				YSSIZE_T toVtIdx;
				if(YSTRUE==YsIsIncluded(toVtIdx,plVtHd,toVtHd))
				{
					// Can it be split between fromVtIdx-toVtIdx?
					YsArray <YsVec3,4> plVtPos;
					shl.GetVertexListOfPolygon(plVtPos,plHd);


					// Mostlikely the remaining edge is a part of numerically-difficult polygons.
					// Expand the polygon so that the aspect ratio is not too large, and then check sepratability >>
					YsVec3 nom;
					if(YSOK!=YsFindLeastSquareFittingPlaneNormal(nom,plVtPos.GetN(),plVtPos))
					{
						break;
					}
					YsVec3 cen=YsGetCenter(plVtPos.GetN(),plVtPos);

					YsMatrix4x4 toXY;
					toXY.MakeToXY(cen,nom);

					YsArray <YsVec2,4> prjPlVtPos;
					for(YSSIZE_T idx=0; idx<plVtPos.GetN(); ++idx)
					{
						YsVec3 prj;
						toXY.Mul(prj,plVtPos[idx],1.0);
						prjPlVtPos.Increment();
						prjPlVtPos.Last().Set(prj.x(),prj.y());
					}

					YSSIZE_T longestIdx=0;
					double longest=0.0;
					for(YSSIZE_T idx=0; idx<prjPlVtPos.GetN(); ++idx)
					{
						auto p1=prjPlVtPos[idx];
						auto p2=prjPlVtPos.GetCyclic(idx+1);
						const double l=(p1-p2).GetSquareLength();
						if(l>longest)
						{
							longestIdx=idx;
							longest=l;
						}
					}

					{
						auto p1=prjPlVtPos[longestIdx];
						auto p2=prjPlVtPos.GetCyclic(longestIdx+1);
						auto d=p2-p1;
						auto angle=atan2(d.y(),d.x());
						for(YSSIZE_T idx=0; idx<prjPlVtPos.GetN(); ++idx)
						{
							prjPlVtPos[idx].Rotate(-angle);
						}
						YsBoundingBoxMaker2 mkBbx;
						mkBbx.Make(prjPlVtPos.GetN(),prjPlVtPos);
						YsVec2 min,max;
						mkBbx.Get(min,max);
						auto dim=max-min;
						auto cen=(min+max)/2.0;
						auto scaleY=dim.x()/dim.y();
						for(YSSIZE_T idx=0; idx<prjPlVtPos.GetN(); ++idx)
						{
							prjPlVtPos[idx]-=cen;
							prjPlVtPos[idx].MulY(scaleY);
						}
					}
					// Mostlikely the remaining edge is a part of numerically-difficult polygons.
					// Expand the polygon so that the aspect ratio is not too large, and then check sepratability <<

					if(YSTRUE==YsCheckSeparatability2(prjPlVtPos.GetN(),prjPlVtPos,(int)fromVtIdx,(int)toVtIdx))
					{
						printf("Recover (%s)-(%s)\n",plVtPos[fromVtIdx].Txt(),plVtPos[toVtIdx].Txt());

						YsArray <YsShellVertexHandle,4> newPlVtHd[2];

						const auto id1=YsSmaller(fromVtIdx,toVtIdx);
						const auto id2=YsGreater(fromVtIdx,toVtIdx);
						for(YSSIZE_T i=0; i<=id1; i++)
						{
							newPlVtHd[0].Append(plVtHd[i]);
						}
						for(YSSIZE_T i=id2; i<plVtHd.GetN(); i++)
						{
							newPlVtHd[0].Append(plVtHd[i]);
						}
						for(YSSIZE_T i=id1; i<=id2; i++)
						{
							newPlVtHd[1].Append(plVtHd[i]);
						}

						if(3<=newPlVtHd[0].GetN() &&
						   3<=newPlVtHd[1].GetN())
						{
							shl.ModifyPolygon(plHd,newPlVtHd[0].GetN(),newPlVtHd[0]);

							auto newPlHd=shl.AddPolygonH(newPlVtHd[1].GetN(),newPlVtHd[1]);
							shl.GetPolygon(newPlHd)->freeAttribute0=shl.GetPolygon(plHd)->freeAttribute0;
							shl.GetPolygon(newPlHd)->freeAttribute1=shl.GetPolygon(plHd)->freeAttribute1;
							shl.GetPolygon(newPlHd)->freeAttribute2=shl.GetPolygon(plHd)->freeAttribute2;
							shl.GetPolygon(newPlHd)->freeAttribute3=shl.GetPolygon(plHd)->freeAttribute3;
							shl.GetPolygon(newPlHd)->freeAttribute4=shl.GetPolygon(plHd)->freeAttribute4;
							goto RETRY;
						}
					}
				}
			}
		}
	}

	ctr.EndCounter();
}

////////////////////////////////////////////////////////////

YSRESULT YsBlendShell2(
    YsShell &newShell,const YsShell &shl0,const YsShell &shl1,YSBOOLEANOPERATION boolop,
    int ltcX,int ltcY,int ltcZ,int /*ltcHashSize*/)
{
	if(boolop==YSBOOLSEPARATE)
	{
		return YSERR;
	}
	YsShell dummy;
	return YsBlendShell3(newShell,dummy,shl0,shl1,boolop,(ltcX+1)*(ltcY+1)*(ltcZ+1));
}


YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,const YsShell &shl0,const YsShell &shl1,YSBOOLEANOPERATION boolop,int nLtcCell)
{
	YsShellLattice lat0,lat1;
	lat0.EnableAutoResizingHashTable();
	lat1.EnableAutoResizingHashTable();

	lat0.SetDomain(shl0,nLtcCell);
	lat1.SetDomain(shl1,nLtcCell);

	return YsBlendShell3(newShell,newShell2,shl0,lat0,shl1,lat1,boolop,nLtcCell);
}

YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,const YsShell &shl0,const YsShellLattice &lat0,const YsShell &shl1,const YsShellLattice &lat1,YSBOOLEANOPERATION boolop,int nLtcCell)
{
	YsShellLattice newShellLtc;

	newShellLtc.EnableAutoResizingHashTable();

	YsPrintf("<<YsBlendShell3>>\n");
	newShell.CleanUp();
	YsDumpOldShellIntoNewShell(newShell,shl0,0);
	YsDumpOldShellIntoNewShell(newShell,shl1,1);
	newShellLtc.SetDomain(newShell,nLtcCell);

	return YsBlendShell3(newShell,newShell2,newShellLtc,shl0,lat0,shl1,lat1,boolop);
}

YSRESULT YsBlendShell3(
    YsShell &newShell,YsShell &newShell2,YsShellLattice &newShellLtc,const YsShell &shl0,const YsShellLattice &lat0,const YsShell &shl1,const YsShellLattice &lat1,YSBOOLEANOPERATION boolop)
{
	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif



	YsShellSearchTable *temporarySearchTable;
	YsShellLattice &ltc=newShellLtc;

	newShell.AutoComputeNormalAll(YSTRUE,YSTRUE);

	if(newShell.GetSearchTable()!=NULL)
	{
		temporarySearchTable=NULL;
	}
	else
	{
		temporarySearchTable=new YsShellSearchTable(65533);
		newShell.AttachSearchTable(temporarySearchTable);
	}
	int nNewVtx1,nNewVtx2,edgeFaceIteration;
	edgeFaceIteration=1;
	for(;;) // Visual C++ complains for "while(1)"
	{
		YsBlendShell2EdgeFaceIntersection(nNewVtx1,newShell,0,ltc,0.999962);
		YsBlendShell2EdgeFaceIntersection(nNewVtx2,newShell,1,ltc,0.999962);

		if(nNewVtx1==0 && nNewVtx2==0)
		{
			break;
		}
		edgeFaceIteration++;
		YsPrintf("Edge-face intersection going to next iteration.\n");
		YsPrintf("(%d)\n",edgeFaceIteration);
	}

	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif


	YsBlendShell2EdgeEdgeIntersection(newShell,ltc,YSFALSE);


	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif


	YsBlendShell2OverlappingPolygon(newShell,ltc,0.999962,YSFALSE);  // Allow 0.5 degree (cos(0.5deg)=0.999962), singleShellOp=YSFALSE


	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif


	YsBlendShell2RemoveOverlapBasedOnTopology(newShell,ltc,YSFALSE);


	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif


	YsBlendShell2SlashFaceByFace(newShell,ltc,YSFALSE);

	#ifdef YSDEBUG_STEP_BY_STEP_OUTPUT
	{
		YsString fn;
		fn.Printf("%s_%d.srf",__FUNCTION__,__LINE__);
		newShell.SaveSrf(fn);
	}
	#endif

	int i,ids[]={8,13};
	YsShellPolygonHandle plHdTest;
	for(i=0; i<0; i++)
	{
		int j;
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		plHdTest=newShell.GetPolygonHandleFromId(ids[i]);
		if(plHdTest!=NULL)
		{
			YsPrintf("ID=%d key=%d\n",ids[i],newShell.GetSearchKey(plHdTest));
			nPlVt=newShell.GetNumVertexOfPolygon(plHdTest);
			plVtHd=newShell.GetVertexListOfPolygon(plHdTest);
			for(j=0; j<nPlVt; j++)
			{
				YsPrintf("  %d",newShell.GetSearchKey(plVtHd[j]));
			}
			YsPrintf("\n");
		}
		else
		{
			YsPrintf("Polygon ID#%d does not exist.\n",ids[i]);
		}
	}

	int vtIds[3]={62,61,73};
	YsShellVertexHandle vtHdTest;
	for(i=0; i<0; i++)
	{
		vtHdTest=newShell.GetVertexHandleFromId(vtIds[i]);
		if(vtHdTest!=NULL)
		{
			YsPrintf("ID=%d key=%d\n",vtIds[i],newShell.GetSearchKey(vtHdTest));
		}
		else
		{
			YsPrintf("Vertex ID#%d does not exist.\n",vtIds[i]);
		}
	}

	YsBlendShell_AttemptToRecoverFailedEdge(newShell);

	YSRESULT res;
 	res=YSERR;

	switch(boolop)
	{
	case YSBOOLBLEND:
		res=YSOK;
		break;
	case YSBOOLAND:
		res=YsBlendAnd2(newShell,shl0,lat0,shl1,lat1);
		break;
	case YSBOOLOR:
		res=YsBlendOr2(newShell,shl0,lat0,shl1,lat1);
		break;
	case YSBOOLMINUS:
		res=YsBlendMinus2(newShell,shl0,lat0,shl1,lat1);
		break;
	case YSBOOLSEPARATE:
		newShell2=newShell;
		{
			YSRESULT res0=YsBlendMinus2(newShell,shl0,lat0,shl1,lat1);
			YSRESULT res1=YsBlendAnd2(newShell2,shl0,lat0,shl1,lat1);
			if(YSOK==res0 && YSOK==res1)
			{
				res=YSOK;
			}
			else
			{
				res=YSERR;
			}
		}
		break;
	case YSBOOL_OPENMINUSSOLID:  /*!< Open surface minus solid. */ 
		res=YsShellBoolOpenMinusSolid(newShell,shl1,lat1);
		break;
	case YSBOOL_OPENTIMESSOLID: /*!< Open surface AND solid. */ 
		res=YsShellBoolOpenTimesSolid(newShell,shl1,lat1);
		break;
	case YSBOOL_OPENPLUSSOLID: /*!< Open surface plus solid. */ 
		res=YsShellBoolOpenPlusSolid(newShell,shl1,lat1);
		break;
	}

	if(temporarySearchTable!=NULL)
	{
		newShell.DetachSearchTable();
		delete temporarySearchTable;
	}

	return res;
}

class YsShellFreeIntersectionRemover
{
public:
	YSRESULT Run(YsShellVertexStore &itscVtx,YsShell &newShell,const YsShell &shl0,int nLtcCell);
private:
	class VtxUsedByUpToTwoSrcPolygon
	{
	public:
		YsShellVertexHandle vtHd;
		int srcPlId[2];
	};
	void FilterAfterRemoval(YsShell &newShell,const YsShellVertexStore &orgVtx);
	YsArray <VtxUsedByUpToTwoSrcPolygon> FindNewVertexUsedFromUpToTwoSourcePolygon(YsShell &newShell,const YsShellVertexStore &orgVtx) const;
	YSBOOL IsVertexUsedFromUpToTwoSourcePolygon(int srcPlId[2],YsShell &newShell,YsShellVertexHandle vtHd) const;
	YSRESULT FindTwoPossibleCollapsingTarget(YsShellVertexHandle tgtVtHd[2],const YsShell &newShell,YsShellVertexHandle fromVtHd,const int srcPlId[2]) const;
	YSBOOL IsEdgeUsedUpToTwoSourcePolygon(YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,const YsShell &newShell,const int srcPlId[]) const;
};

YSRESULT YsShellRemoveFreeIntersection(YsShellVertexStore &itscVtx,YsShell &newShell,const YsShell &shl0,int nLtcCell)
{
	YsShellFreeIntersectionRemover remover;
	return remover.Run(itscVtx,newShell,shl0,nLtcCell);
}

YSRESULT YsShellFreeIntersectionRemover::Run(YsShellVertexStore &itscVtx,YsShell &newShell,const YsShell &shl0,int nLtcCell)
{
	YsShellSearchTable *temporarySearchTable;
	YsShellLattice ltc,lat0;

	itscVtx.CleanUp();
	itscVtx.SetShell(newShell);

	ltc.EnableAutoResizingHashTable();

	YsPrintf("<<%s>>\n",__FUNCTION__);
	newShell.CleanUp();
	YsDumpOldShellIntoNewShell(newShell,shl0,0);
	ltc.SetDomain(newShell,nLtcCell);

	lat0.SetDomain(shl0,nLtcCell);

	newShell.AutoComputeNormalAll(YSTRUE,YSTRUE);

	YsShellVertexStore orgVtx(newShell);
	for(auto iter=newShell.AllVertex().begin() ; iter!=newShell.AllVertex().end() ; ++iter)
	{
		orgVtx.AddVertex(*iter);
	}

	if(newShell.GetSearchTable()!=NULL)
	{
		temporarySearchTable=NULL;
	}
	else
	{
		temporarySearchTable=new YsShellSearchTable(65533);
		newShell.AttachSearchTable(temporarySearchTable);
	}
	int nNewVtx1,edgeFaceIteration;
	edgeFaceIteration=1;
	for(;;) // Visual C++ complains for "while(1)"
	{
		YsBlendShell2EdgeFaceIntersection(nNewVtx1,newShell,-1,ltc,0.999962); // Source shell ID=-1 means single-shell operation.
		if(nNewVtx1==0)
		{
			break;
		}
		edgeFaceIteration++;
		YsPrintf("Edge-face intersection going to next iteration.\n");
		YsPrintf("(%d)\n",edgeFaceIteration);
	}

	YsBlendShell2EdgeEdgeIntersection(newShell,ltc,YSTRUE);
	YsBlendShell2OverlappingPolygon(newShell,ltc,0.999962,YSTRUE);  // Allow 0.5 degree (cos(0.5deg)=0.999962), singleShellOp=YSTRUE
	YsBlendShell2RemoveOverlapBasedOnTopology(newShell,ltc,YSTRUE); // singleShellOp=YSTRUE;
	YsBlendShell2SlashFaceByFace(newShell,ltc,YSTRUE);

	YsBlendShell_AttemptToRecoverFailedEdge(newShell);

	FilterAfterRemoval(newShell,orgVtx);

	for(auto iter=newShell.AllVertex().begin(); iter!=newShell.AllVertex().end(); ++iter)
	{
		if(YSTRUE!=orgVtx.IsIncluded(*iter))
		{
			itscVtx.AddVertex(*iter);
		}
	}

	if(temporarySearchTable!=NULL)
	{
		newShell.DetachSearchTable();
		delete temporarySearchTable;
	}

	return YSOK;
}

void YsShellFreeIntersectionRemover::FilterAfterRemoval(YsShell &newShell,const YsShellVertexStore &orgVtx)
{
	auto search=newShell.GetSearchTable();

	auto collapsibleVtHdArray=FindNewVertexUsedFromUpToTwoSourcePolygon(newShell,orgVtx);
	for(auto iter=collapsibleVtHdArray.begin(); collapsibleVtHdArray.end()!=iter; ++iter)
	{
		auto &collapsible=*iter;

		if(YsVec3(6524.0313720000, 941.1655130000, -4086.2883430000)==newShell.GetVertexPosition(collapsible.vtHd))
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
		}

		YsShellVertexHandle tgtVtHd[2];
		if(YSOK==FindTwoPossibleCollapsingTarget(tgtVtHd,newShell,collapsible.vtHd,collapsible.srcPlId))
		{
			printf("From %s\n",newShell.GetVertexPosition(collapsible.vtHd).Txt());
			if(NULL!=tgtVtHd[0])
			{
				printf("  To %s\n",newShell.GetVertexPosition(tgtVtHd[0]).Txt());
			}
			if(NULL!=tgtVtHd[1])
			{
				printf("  To %s\n",newShell.GetVertexPosition(tgtVtHd[1]).Txt());
			}

			{
				// See 20140410-boolean.jnt
				// Safest possible case.  The vertex is connected to only two vertices.  (Automatically used by up to two polygons.)
				// In this case, the vertex and the two connected vertices are supposed to be on a straight line
				// becasue the vertex is on two source polygons,
				// and since no other edges are connected to the vertex, deleting the vertex will never create a 
				// tangled polygon.
				YsArray <YsShellVertexHandle,16> connVtHd;
				if(YSOK==search->GetConnectedVertexList(connVtHd,newShell,collapsible.vtHd) &&
				   2==connVtHd.GetN() &&
				   YSTRUE==YsSameEdge(connVtHd[0],connVtHd[1],tgtVtHd[0],tgtVtHd[1]))
				{
					printf("*\n");

					auto vtPlHdPtr=search->FindPolygonListByVertex(newShell,collapsible.vtHd);
					if(NULL!=vtPlHdPtr)
					{
						auto vtPlHd=*vtPlHdPtr;
						for(auto iter=vtPlHd.begin(); vtPlHd.end()!=iter; ++iter)
						{
							auto plHd=*iter;

							YsArray <YsShellVertexHandle,4> plVtHd;
							newShell.GetPolygon(plVtHd,plHd);
							for(YSSIZE_T plVtIdx=plVtHd.GetN()-1; 0<=plVtIdx; --plVtIdx)
							{
								if(plVtHd[plVtIdx]==collapsible.vtHd)
								{
									plVtHd.Delete(plVtIdx);
								}
							}
							if(3<=plVtHd.GetN())
							{
								newShell.ModifyPolygon(plHd,plVtHd);
							}
							else
							{
								newShell.DeletePolygon(plHd);
							}
						}
					}
				}
			}
		}
	}
}

YsArray <YsShellFreeIntersectionRemover::VtxUsedByUpToTwoSrcPolygon> YsShellFreeIntersectionRemover::FindNewVertexUsedFromUpToTwoSourcePolygon(YsShell &newShell,const YsShellVertexStore &orgVtx) const
{
	YsArray <VtxUsedByUpToTwoSrcPolygon> vtHdArray;
	for(auto iter=newShell.AllVertex().begin(); iter!=newShell.AllVertex().end(); ++iter)
	{
		int srcPlId[2];
		if(YSTRUE!=orgVtx.IsIncluded(*iter) && YSTRUE==IsVertexUsedFromUpToTwoSourcePolygon(srcPlId,newShell,*iter))
		{
			vtHdArray.Increment();
			vtHdArray.Last().vtHd=*iter;
			vtHdArray.Last().srcPlId[0]=srcPlId[0];
			vtHdArray.Last().srcPlId[1]=srcPlId[1];
		}
	}
	return vtHdArray;
}

YSBOOL YsShellFreeIntersectionRemover::IsVertexUsedFromUpToTwoSourcePolygon(int srcPlId[2],YsShell &newShell,YsShellVertexHandle vtHd) const
{
	auto search=newShell.GetSearchTable();
	auto vtPlHdArray=search->FindPolygonListByVertex(newShell,vtHd);

	if(NULL!=vtPlHdArray)
	{
		srcPlId[0]=-1;
		srcPlId[1]=-1;
		for(auto iter=vtPlHdArray->begin(); iter!=vtPlHdArray->end(); ++iter)
		{
			auto plg=newShell.GetPolygon(*iter);
			if(0==plg->freeAttribute2)
			{
				const auto plId=plg->freeAttribute3;
				if((0<=srcPlId[0] && srcPlId[0]==plId) || (0<=srcPlId[1] && srcPlId[1]==plId))
				{
					// Nothing to do
				}
				else if(0>srcPlId[0])
				{
					srcPlId[0]=plId;
				}
				else if(0>srcPlId[1])
				{
					srcPlId[1]=plId;
				}
				else
				{
					return YSFALSE;
				}
			}
			else
			{
				// Maybe overlapping.
				return YSFALSE;
			}
		}
		if(0<=srcPlId[0])
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSRESULT YsShellFreeIntersectionRemover::FindTwoPossibleCollapsingTarget(YsShellVertexHandle tgtVtHd[2],const YsShell &newShell,YsShellVertexHandle fromVtHd,const int srcPlId[2]) const
{
	auto search=newShell.GetSearchTable();

	YsArray <YsShellVertexHandle,16> connVtHd;
	search->GetConnectedVertexList(connVtHd,newShell,fromVtHd);

	int nCollapseTarget=0;
	tgtVtHd[0]=NULL;
	tgtVtHd[1]=NULL;

	for(auto iter=connVtHd.begin(); iter!=connVtHd.end(); ++iter)
	{
		if(YSTRUE==IsEdgeUsedUpToTwoSourcePolygon(fromVtHd,*iter,newShell,srcPlId))
		{
			if(2>nCollapseTarget)
			{
				tgtVtHd[nCollapseTarget]=*iter;
				++nCollapseTarget;
			}
			else
			{
				return YSERR;
			}
		}
	}
	if(0<nCollapseTarget)
	{
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellFreeIntersectionRemover::IsEdgeUsedUpToTwoSourcePolygon(YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,const YsShell &newShell,const int srcPlId[]) const
{
	int nEdPl;
	const YsShellPolygonHandle *edPlHd;

	auto search=newShell.GetSearchTable();
	if(YSOK==search->FindPolygonListByEdge(nEdPl,edPlHd,newShell,fromVtHd,toVtHd))
	{
		int nEdSrcPlId=0;
		int edSrcPlId[2]={-1,-1};
		for(YSSIZE_T edPlIdx=0; edPlIdx<nEdPl; ++edPlIdx)
		{
			auto plg=newShell.GetPolygon(edPlHd[edPlIdx]);
			if(0==plg->freeAttribute2)
			{
				const auto plId=plg->freeAttribute3;
				if(YSTRUE!=YsIsIncluded(nEdSrcPlId,edSrcPlId,plId))
				{
					if(2>nEdSrcPlId)
					{
						edSrcPlId[nEdSrcPlId]=plId;
						++nEdSrcPlId;
					}
					else
					{
						return YSFALSE;
					}
				}
			}
			else
			{
				return YSFALSE;
			}
		}

		if(YSTRUE==YsSameEdge(edSrcPlId[0],edSrcPlId[1],srcPlId[0],srcPlId[1]))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

YSRESULT YsBlendShellPolygon
    (YsArray <YsShellPolygonHandle> &newPlHd,YsShell &shl,const YsShellPolygonHandle plHd[2],YsShellLattice *givenLtc)
{
	// CheckAndKillCLoop -> Checks if plHd is one of samePlnPlHd, and if samePlnPlHd[n] includes
	//                      plHd inside, i.e., plHd is a C-Loop of samePlnPlHd[n], samePlnPlHd[n]
	//                      is subdivided into two so that plHd is no longer a C-Loop
	YsArray <YsShellPolygonHandle,16> plHdGroup[2];
	YsArray <YsShellPolygonHandle,16> createdPlHd;
	YsShellLattice *usingLtc;
	YsVec3 nom;
	YsAtt3 prjAtt;
	YsMatrix4x4 prjMat;

	newPlHd.Set(0,NULL);
	/* shl.GetPolygon(plHd[0])->freeAttribute2=0;
	shl.GetPolygon(plHd[0])->freeAttribute3=0;
	shl.GetPolygon(plHd[0])->freeAttribute4=0;
	shl.GetPolygon(plHd[1])->freeAttribute2=1;
	shl.GetPolygon(plHd[1])->freeAttribute3=0;
	shl.GetPolygon(plHd[1])->freeAttribute4=0; */

	const YSBOOL singleShellOp=YSTRUE;


	YsShellSearchTable *temporarySearchTable;
	if(shl.GetSearchTable()!=NULL)
	{
		temporarySearchTable=NULL;
	}
	else
	{
		temporarySearchTable=new YsShellSearchTable(65533);
		shl.AttachSearchTable(temporarySearchTable);
	}

	YsShellLattice *temporaryLattice;
	if(givenLtc!=NULL)
	{
		temporaryLattice=NULL;
		usingLtc=givenLtc;
	}
	else
	{
		temporaryLattice=new YsShellLattice(251);
		usingLtc=temporaryLattice;
		usingLtc->SetDomain(shl,16,16,16);
	}

	YsShellLattice &ltc=*usingLtc;


	plHdGroup[0].Append(plHd[0]);
	plHdGroup[1].Append(plHd[1]);

	if(YsBlendShell2GetNormalOfPolygon(nom,shl,plHd[0])!=YSOK &&
	   YsBlendShell2GetNormalOfPolygon(nom,shl,plHd[1])!=YSOK)
	{
		YsErrOut("YsBlendShellPolygon()\n");
		YsErrOut("  Cannot compute normal.\n");
		return YSERR;
	}
	prjAtt.SetForwardVector(nom);
	prjAtt.GetMatrix4x4(prjMat);



	YsBlendShell2StitchNodeOfOverlapPolygon(shl,plHd[0],plHd[1],prjMat,ltc);
	YsBlendShell2StitchEdgeOfOverlapPolygon(shl,plHd[1],plHd[0],prjMat,ltc);

	YsPrintf("Dealing with C-Loops\n");
	int i,j,k,target,cutter;
	for(i=0; i<2; i++)
	{
		target=i;
		cutter=(i+1)%2;

		for(j=0; j<plHdGroup[target].GetN(); j++)
		{
		RETRY:
			for(k=0; k<plHdGroup[cutter].GetN(); k++)
			{
				if(YsBlendShell2CheckAndKillCLoop
				    (createdPlHd,shl,ltc,plHdGroup[cutter][k],plHdGroup[target][j],prjMat,singleShellOp)==YSTRUE)
				{
					int m;
					plHdGroup[target].DeleteBySwapping(j);
					for(m=0; m<createdPlHd.GetN(); m++)
					{
						plHdGroup[target].Append(createdPlHd[m]);

						int n;
						for(n=0; n<plHdGroup[cutter].GetN(); n++)
						{
							YsBlendShell2StitchNodeOfOverlapPolygon
							    (shl,plHdGroup[cutter][n],createdPlHd[m],prjMat,ltc);
							YsBlendShell2StitchEdgeOfOverlapPolygon
							    (shl,createdPlHd[m],plHdGroup[cutter][n],prjMat,ltc);
						}
					}
					goto RETRY;
				}
			}
		}
	}

	YsPrintf("Dealing with Intersections\n");
	YsArray <YsArray <YsShellVertexHandle,16> > newPlVtHdList;
	YsArray <YsArray <YsVec2,16> > newPlVtPosList;
	YsArray <YsShellVertexHandle,16> targetVtHd;
	YsArray <YsVec2,16> targetVtPos;
	YsArray <YsShellVertexHandle,16> cutterVtHd;
	YsArray <YsVec2,16> cutterVtPos;
	YSBOOL split,watch;

	watch=YSFALSE;
	for(i=0; i<2; i++)
	{
		YsPrintf("(%d)\n",i);

		target=i;
		cutter=(i+1)%2;

		j=0;
		while(j<plHdGroup[target].GetN())
		{
			split=YSFALSE;
			YsBlendShell2GetProjectedPolygon(targetVtHd,targetVtPos,shl,plHdGroup[target][j],prjMat);
			for(k=0; k<plHdGroup[cutter].GetN(); k++)
			{
				YsBlendShell2GetProjectedPolygon(cutterVtHd,cutterVtPos,shl,plHdGroup[cutter][k],prjMat);

				if(watch==YSTRUE)
				{
					YsPrintf("[%04d](%d) by [%04d](%d)\n",
					    shl.GetSearchKey(plHdGroup[target][j]),shl.GetNumVertexOfPolygon(plHdGroup[target][j]),
					    shl.GetSearchKey(plHdGroup[cutter][k]),shl.GetNumVertexOfPolygon(plHdGroup[cutter][k]));

					int m;
					for(m=0; m<targetVtHd.GetN(); m++)
					{
						YsPrintf("[%d]",shl.GetSearchKey(targetVtHd[m]));
					}
					YsPrintf("\n");
					for(m=0; m<cutterVtHd.GetN(); m++)
					{
						YsPrintf("[%d]",shl.GetSearchKey(cutterVtHd[m]));
					}
					YsPrintf("\n");
				}

				newPlVtHdList.Set(0,NULL);
				newPlVtPosList.Set(0,NULL);
				YsShellBlend2SubdivOverlappingPolygon
				    (newPlVtHdList,newPlVtPosList,shl,targetVtHd,targetVtPos,cutterVtHd,cutterVtPos,watch);
				if(newPlVtHdList.GetN()>1)
				{
					split=YSTRUE;

					int m;
					YsVec3 nom;
					YsColor col;
					int att0,att1,att2,att3,att4;
					shl.GetNormalOfPolygon(nom,plHdGroup[target][j]);
					shl.GetColorOfPolygon(col,plHdGroup[target][j]);
					att0=shl.GetPolygon(plHdGroup[target][j])->freeAttribute0; // 2002/12/24
					att1=shl.GetPolygon(plHdGroup[target][j])->freeAttribute1; // 2002/12/24
					att2=shl.GetPolygon(plHdGroup[target][j])->freeAttribute2;
					att3=shl.GetPolygon(plHdGroup[target][j])->freeAttribute3;
					att4=shl.GetPolygon(plHdGroup[target][j])->freeAttribute4;
					for(m=0; m<newPlVtHdList.GetN(); m++)
					{
						YsShellPolygonHandle plHd;
						plHd=shl.AddPolygonH(newPlVtHdList[m].GetN(),newPlVtHdList[m]);
						shl.SetNormalOfPolygon(plHd,nom);
						shl.SetColorOfPolygon(plHd,col);
						shl.GetPolygon(plHd)->freeAttribute0=att0; // 2002/12/24
						shl.GetPolygon(plHd)->freeAttribute1=att1; // 2002/12/24
						shl.GetPolygon(plHd)->freeAttribute2=att2;
						shl.GetPolygon(plHd)->freeAttribute3=att3;
						shl.GetPolygon(plHd)->freeAttribute4=att4;
						plHdGroup[target].Append(plHd);
						ltc.AddPolygon(plHd);

						if(watch==YSTRUE)
						{
							YsPrintf(" -> key=%d, nVt=%d\n",shl.GetSearchKey(plHd),(int)newPlVtHdList[m].GetN());
						}
					}
					ltc.DeletePolygon(plHdGroup[target][j]);
					shl.DeletePolygon(plHdGroup[target][j]);
					plHdGroup[target].DeleteBySwapping(j);

					break; // <<!!!!
				}
			}
			if(split!=YSTRUE)
			{
				j++;
			}
		}
	}


	YsPrintf("Eliminating Overlap\n");
	for(i=0; i<2; i++)
	{
		for(j=0; j<plHdGroup[i].GetN(); j++)
		{
			newPlHd.Append(plHdGroup[i][j]);
		}
	}
	YsBlendShell2RemoveOverlapBasedOnTopology(shl,ltc,singleShellOp);

	if(temporarySearchTable!=NULL)
	{
		shl.DetachSearchTable();
		delete temporarySearchTable;
	}

	if(temporaryLattice!=NULL)
	{
		delete temporaryLattice;
	}


	int ids[2]={2,3};
	for(i=0; i<0; i++)
	{
		YsShellPolygonHandle plHd;
		plHd=shl.GetPolygonHandleFromId(ids[i]);
		YsPrintf("(%d) ID=%d,key=%d,nVt=%d\n",
		    i,ids[i],shl.GetSearchKey(plHd),shl.GetNumVertexOfPolygon(plHd));
	}


	return YSOK;
}



////////////////////////////////////////////////////////////

// For single-shell operation, cutterShlId=-1
static YSRESULT YsBlendShell2EdgeFaceIntersection
    (int &nNewVtx,YsShell &newShell,int cutterShlId,YsShellLattice &ltc,const double &cosThr)
{
	#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
	const YsVec3 watchEdVtPos[2]=
	{
		YsVec3(-0.1326970000, 14.3775280000, -0.4038460000),
		YsVec3( 0.1004080000, 14.4356770000, -1.6284290000)
	};
	// Step 1 Save temporary srf
	// newShell.SaveSrf("c:/tmp/test.srf");
	// printf("%s %d>",__FUNCTION__,__LINE__);getchar();
	// Step 2 Find ID of the cutter polygon in ysgebl
	// 3434
	// Step 3 Find key
	//auto provePlHd=newShell.GetPolygonHandleFromId(3434);
	//printf("PlKey=%d\n",newShell.GetSearchKey(provePlHd));
	//newShell.SetColorOfPolygon(provePlHd,YsGreen());
	//newShell.SaveSrf("c:/tmp/test.srf");
	//printf("%s %d>",__FUNCTION__,__LINE__);getchar();
	// Step 4 Verify the polygon is painted green in ysgebl.
	const YSHASHKEY watchCutterPlKey=YSNULLHASHKEY;
	const YSHASHKEY watchBeingCutPlKey=YSNULLHASHKEY;
	#endif


	YsShellPolygonHandle plHd;
	YsArray <YSHASHKEY> cutterPlKeyList;
	YsArray <YsVec3> plVtPos;
	YsArray <YsVec2,16> plVtPrj;
	YsAtt3 projection;
	YsVec3 nom;
	YsArray <YsShellPolygonHandle,16> collidePlHd;
	YsPlane cutterPln;
	YsVec3 p;
	YsVec2 p2;
	YsArray <YsShellPolygonHandle,16> createdPlHd;
	YSBOOL watch;

	auto search=newShell.GetSearchTable();

	const int edgeShlId=(0<=cutterShlId ? ((cutterShlId+1)&1) : -1);

	nNewVtx=0;
	watch=YSFALSE;

	plHd=NULL;
	while((plHd=newShell.FindNextPolygon(plHd))!=NULL)
	{
		YsShellPolygon *plg=newShell.GetPolygon(plHd);

		#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
		if(watchCutterPlKey==newShell.GetSearchKey(plHd))
		{
			printf("Cutter PlKey %d\n",newShell.GetSearchKey(plHd));
			printf("Polygon ShlID %d  Cutter ShlId %d\n",plg->freeAttribute2,cutterShlId);
			printf("%s %d>",__FUNCTION__,__LINE__);getchar();
		}
		#endif

		if(plg->freeAttribute2==cutterShlId || -1==cutterShlId)
		{
			cutterPlKeyList.Append(newShell.GetSearchKey(plHd));
		}
	}



	#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
	auto watchCutterPlHd=search->FindPolygon(newShell,watchCutterPlKey);
	if(NULL!=watchCutterPlHd)
	{
		printf("%s %d>",__FUNCTION__,__LINE__);getchar();
	}
	#endif



	YsPrintf("Computing EdgeFace Intersection....\n");
	int i,j;
	YsLoopCounter ctr;
	ctr.BeginCounter(cutterPlKeyList.GetN());
	for(i=0; i<cutterPlKeyList.GetN(); i++)
	{
		ctr.Increment();

		auto cutterPlHd=search->FindPolygon(newShell,cutterPlKeyList[i]);

		#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
		if(NULL!=watchCutterPlHd && NULL==search->FindPolygon(newShell,watchCutterPlKey))
		{
			printf("Watch cutter polygon is NULL at i=%d\n",i);
			printf("%s %d>",__FUNCTION__,__LINE__);getchar();
		}
		#endif

		if(NULL==cutterPlHd)
		{
			continue;
		}

		#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
		if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd))
		{
			newShell.SetColorOfPolygon(cutterPlHd,YsGreen());
			newShell.SaveSrf("c:/tmp/test.srf");
			printf("%s %d>",__FUNCTION__,__LINE__);getchar();
		}
		#endif

		newShell.GetVertexListOfPolygon(plVtPos,cutterPlHd);
		newShell.GetNormalOfPolygon(nom,cutterPlHd);
		if(nom==YsOrigin())
		{
			if(YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos)!=YSOK)
			{
				YsErrOut("\n");
				YsErrOut("Cannot compute a normal for a polygon.  Skipping it.\n");
				goto NEXTPOLYGON;
			}
		}

		cutterPln.Set(plVtPos[0],nom);

		plVtPrj.Set(0,NULL);
		projection.SetForwardVector(nom);
		for(j=0; j<plVtPos.GetN(); j++)
		{
			projection.MulInverse(p,plVtPos[j]);
			p2.Set(p.x(),p.y());
			plVtPrj.Append(p2);
		}

		if(ltc.MakePossibllyCollidingPolygonList(collidePlHd,plVtPos.GetN(),plVtPos)==YSOK)
		{
			#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
			if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd))
			{
				for(auto plHd : newShell.AllPolygon())
				{
					newShell.SetColorOfPolygon(plHd,YsWhite());
				}
				for(auto plHd : collidePlHd)
				{
					newShell.SetColorOfPolygon(plHd,YsBlue());
				}
				newShell.SetColorOfPolygon(cutterPlHd,YsGreen());
				newShell.SaveSrf("c:/tmp/test.srf");
				printf("%s %d>",__FUNCTION__,__LINE__);getchar();
			}
			#endif

			for(j=0; j<collidePlHd.GetN(); j++)
			{
				#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
				if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd))
				{
					YsArray <YsVec3,4> plVtPos;
					newShell.GetPolygon(plVtPos,collidePlHd[j]);
					for(auto idx : plVtPos.AllIndex())
					{
						if(YSTRUE==YsSameEdge(watchEdVtPos[0],watchEdVtPos[1],plVtPos[idx],plVtPos.GetCyclic(idx+1)))
						{
							newShell.SetColorOfPolygon(collidePlHd[j],YsYellow());
							newShell.SaveSrf("c:/tmp/test.srf");
							printf("Key=%d\n",newShell.GetSearchKey(collidePlHd[j])); // Got 6143
							printf("%s %d>",__FUNCTION__,__LINE__);getchar();
						}
					}
				}
				#endif


				if(watch==YSTRUE)
				{
					YsPrintf("Watch! Key(collidePlHd[j])=%d\n",newShell.GetSearchKey(collidePlHd[j]));
					YsPrintf("Watch! %d %d\n",j,(int)collidePlHd.GetN());
				}

				if(newShell.GetPolygon(collidePlHd[j])->freeAttribute2==edgeShlId || (-1==edgeShlId && collidePlHd[j]!=cutterPlHd))
				{
					#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
					if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd) &&
					   watchBeingCutPlKey==newShell.GetSearchKey(collidePlHd[j]))
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
					}
					#endif


					int nCollPlVtHd;
					const YsShellVertexHandle *collPlVtHd;
					YsVec3 collideNom;

					nCollPlVtHd=newShell.GetNumVertexOfPolygon(collidePlHd[j]);
					collPlVtHd=newShell.GetVertexListOfPolygon(collidePlHd[j]);

					newShell.GetNormalOfPolygon(collideNom,collidePlHd[j]);
					if(collideNom==YsOrigin())
					{
						YsArray <YsVec3> collidePlVtPos;
						newShell.GetVertexListOfPolygon(collidePlVtPos,collidePlHd[j]);
						if(YsGetAverageNormalVector(collideNom,collidePlVtPos.GetN(),collidePlVtPos)!=YSOK)
						{
							YsErrOut("\n");
							YsErrOut("Cannot compute a normal for a polygon.  Skipping it.\n");
							goto NEXTPOLYGON;
						}
					}
					if(YsAbs(collideNom*nom)>=cosThr)  // Overlapping or parallel polygon
					{
						if(watch==YSTRUE)
						{
							YsPrintf("Watch! (1)\n");
						}
						goto NEXTJ;   // 2002/05/09
					}


					#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
					if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd) &&
					   watchBeingCutPlKey==newShell.GetSearchKey(collidePlHd[j]))
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
					}
					#endif


					// One of the vertices of the edge-shl polygon is lying on the cutter polygon >>
					for(int k=0; k<nCollPlVtHd; k++)
					{
						newShell.GetVertexPosition(p,collPlVtHd[k]);
						if(cutterPln.CheckOnPlane(p)==YSTRUE)
						{
							projection.MulInverse(p,p);
							p2.Set(p.x(),p.y());
							if(YsCheckInsidePolygon2(p2,plVtPrj.GetN(),plVtPrj)==YSINSIDE)
							{
								YsShellBlend2InsertVertexToPolygon
								    (createdPlHd,newShell,cutterPlHd,cutterPlKeyList,collPlVtHd[k],ltc,plVtPrj,p2);
								int i;
								for(i=0; i<createdPlHd.GetN(); i++)
								{
									YsShellBlend2ReconnectEdgeToEliminateSharpCorner
										(newShell,createdPlHd[i],ltc,0.996);  // 0.996=cos(5deg)
								}
								if(watch==YSTRUE)
								{
									YsPrintf("Watch! (2)\n");
								}
								goto NEXTPOLYGON;
							}
						}
					}
					// One of the vertices of the edge-shl polygon is lying on the cutter polygon <<


					#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
					if(watchCutterPlKey==newShell.GetSearchKey(cutterPlHd) &&
					   watchBeingCutPlKey==newShell.GetSearchKey(collidePlHd[j]))
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
					}
					#endif


					for(int k=0; k<nCollPlVtHd; k++)
					{
						YsVec3 edVtPos[2],is;

						newShell.GetVertexPosition(edVtPos[0],collPlVtHd[k]);
						newShell.GetVertexPosition(edVtPos[1],collPlVtHd[(k+1)%nCollPlVtHd]);

						#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
						if(YSTRUE==YsSameEdge(edVtPos[0],edVtPos[1],watchEdVtPos[0],watchEdVtPos[1]))
						{
							printf("CutterPlKey (%d)\n",newShell.GetSearchKey(cutterPlHd));
						}
						#endif

						if(cutterPln.GetIntersection(is,edVtPos[0],(edVtPos[1]-edVtPos[0]))==YSOK &&
						   YsCheckInBetween3(is,edVtPos[0],edVtPos[1])==YSTRUE)
						{
							projection.MulInverse(p,is);
							p2.Set(p.x(),p.y());

							#ifdef YSDEBUG_EDGE_FACE_INTERSECTION
							if(YSTRUE==YsSameEdge(edVtPos[0],edVtPos[1],watchEdVtPos[0],watchEdVtPos[1]))
							{
								for(auto plHd : newShell.AllPolygon())
								{
									newShell.SetColorOfPolygon(plHd,YsWhite());
								}
								newShell.SetColorOfPolygon(collidePlHd[j],YsRed());
								newShell.SetColorOfPolygon(cutterPlHd,YsGreen());
								newShell.SaveSrf("c:/tmp/test.srf");

								printf("%s\n",is.Txt());
								printf("%s %d>",__FUNCTION__,__LINE__);getchar();
							}
							{
								YSBOOL watchMatch=YSFALSE;
								for(auto pos : watchItscPos)
								{
									if(pos==is)
									{
										printf("(%s)(%s)\n",is.Txt(),pos.Txt());
										watchMatch=YSTRUE;
									}
								}
								if(YSTRUE==watchMatch)
								{
									for(auto plHd : newShell.AllPolygon())
									{
										newShell.SetColorOfPolygon(plHd,YsWhite());
									}
									newShell.SetColorOfPolygon(collidePlHd[j],YsRed());
									newShell.SetColorOfPolygon(cutterPlHd,YsGreen());
									newShell.SaveSrf("c:/tmp/test.srf");

									printf("%s\n",is.Txt());
									printf("%s %d>",__FUNCTION__,__LINE__);getchar();
								}
							}
							#endif

							if(YsCheckInsidePolygon2(p2,plVtPrj.GetN(),plVtPrj)==YSINSIDE)
							{
								// 2014/11/11 What if collPlVtHd[k] or collPlVtHd[(k+1)%nCollPlVtHd] is one of the vertices of the cutter polygon? >>
								// See ysgebl/debugdata/20141110-triangulationFailure/memo.txt
								auto cutterPlVtHd=newShell.GetPolygonVertex(cutterPlHd);
								if(YSTRUE==cutterPlVtHd.IsIncluded(collPlVtHd[k]) ||
								   YSTRUE==cutterPlVtHd.IsIncluded(collPlVtHd[(k+1)%nCollPlVtHd]))
								{
									// 2014/12/05  This should be next edge.  Not next polygon.
									goto NEXTK;  // Wrong: goto NEXTPOLYGON;
								}
								// 2014/11/11 What if collPlVtHd[k] or collPlVtHd[(k+1)%nCollPlVtHd] is one of the vertices of the cutter polygon? <<

								if(edVtPos[0]==is)
								{
									YsShellBlend2InsertVertexToPolygon
									    (createdPlHd,newShell,cutterPlHd,cutterPlKeyList,collPlVtHd[k],ltc,plVtPrj,p2);

									YsShellBlend2ReconnectEdgeToEliminateSharpCorner
										(newShell,createdPlHd,ltc,0.996);  // 0.996=cos(5deg)

									if(watch==YSTRUE)
									{
										YsPrintf("Watch! (3)\n");
									}
									goto NEXTPOLYGON;
								}
								else if(edVtPos[1]==is)
								{
									YsShellBlend2InsertVertexToPolygon
									    (createdPlHd,newShell,
									     cutterPlHd,cutterPlKeyList,collPlVtHd[(k+1)%nCollPlVtHd],ltc,plVtPrj,p2);

									YsShellBlend2ReconnectEdgeToEliminateSharpCorner
										(newShell,createdPlHd,ltc,0.996);  // 0.996=cos(5deg)

									if(watch==YSTRUE)
									{
										YsPrintf("Watch! (4)\n");
									}
									goto NEXTPOLYGON;
								}
								else
								{
									YsShellVertex *vtx;
									YsShellVertexHandle newVtHd;
									newVtHd=newShell.AddVertexH(is);

									vtx=newShell.GetVertex(newVtHd);
									vtx->freeAttribute3=-1;  // Made by edge-face intersection
									vtx->freeAttribute4=-1;

									ltc.AddVertex(newVtHd);
									YsShellBlend2InsertVertexToEdge
									    (newShell,collPlVtHd[k],collPlVtHd[(k+1)%nCollPlVtHd],newVtHd,ltc);
									YsShellBlend2InsertVertexToPolygon
									    (createdPlHd,newShell,cutterPlHd,cutterPlKeyList,newVtHd,ltc,plVtPrj,p2);
									YsShellBlend2ReconnectEdgeToEliminateSharpCorner
											(newShell,createdPlHd,ltc,0.996);  // 0.996=cos(5deg)

									nNewVtx++;
									if(watch==YSTRUE)
									{
										YsPrintf("Watch! (5)\n");
									}
									goto NEXTPOLYGON;
								}
							}
						}
						NEXTK:
							;
					}
				}
			NEXTJ:
				;
			}
		}


	NEXTPOLYGON:
		;
	}
	ctr.EndCounter();

	return YSOK;
}

YSRESULT YsShellBlend2InsertVertexToPolygon
    (YsArray <YsShellPolygonHandle,16> &createdPlHd,
     YsShell &newShell,YsShellPolygonHandle cutterPlHd,
     YsArray <YSHASHKEY> &cutterPlKeyList,YsShellVertexHandle vtHd,
     YsShellLattice &ltc,YsArray <YsVec2,16> &plVtPrj,const YsVec2 &p2)
{
	createdPlHd.Set(0,NULL);

	// (1) Split the polygon[cutterPlHd] into two polygons by inserting the vertex[vtHd]
	int i,j,splitterIdx[2];

	splitterIdx[0]=-1;
	splitterIdx[1]=-1;


	YsArray <YSBOOL,16> reach;
	double score;
	for(i=0; i<plVtPrj.GetN(); i++)
	{
		reach.Append(YsShellBlend2CheckReach(p2,plVtPrj.GetN(),plVtPrj,i));
	}
	score=1.0;  // cos(ang)==1.0 -> worst  want to make it close to 180 degree
	for(i=0; i<plVtPrj.GetN(); i++)
	{
		if(reach[i]==YSTRUE)
		{
			for(j=i+1; j<plVtPrj.GetN(); j++)
			{
				if(reach[j]==YSTRUE)
				{
					double newAngle;
					YsVec2 v1[5],v2[5];

					v1[0]=p2-plVtPrj[i];
					v2[0]=p2-plVtPrj[j];

					v1[1]=plVtPrj[i]-plVtPrj[(i+plVtPrj.GetN()-1)%plVtPrj.GetN()];
					v2[1]=plVtPrj[i]-p2;

					v1[2]=plVtPrj[i]-plVtPrj[(i+1)%plVtPrj.GetN()];
					v2[2]=plVtPrj[i]-p2;

					v1[3]=plVtPrj[j]-plVtPrj[(j+plVtPrj.GetN()-1)%plVtPrj.GetN()];
					v2[3]=plVtPrj[j]-p2;

					v1[4]=plVtPrj[j]-plVtPrj[(j+1)%plVtPrj.GetN()];
					v2[4]=plVtPrj[j]-p2;

					int k;
					newAngle=-1.0;
					for(k=0; k<5; k++)
					{
						if(v1[k].Normalize()==YSOK && v2[k].Normalize()==YSOK)
						{
							if(v1[k]*v2[k]>newAngle)
							{
								newAngle=v1[k]*v2[k];
							}
						}
						else
						{
							newAngle=1.0;
							break;
						}
					}

					if(splitterIdx[0]==-1 || newAngle<score)
					{
						splitterIdx[0]=i;
						splitterIdx[1]=j;
						score=newAngle;
					}
				}
			}
		}
	}


	/*  Search algorithm (commented out 2002/04/06)
	int baseIdx;
	for(i=0; i<plVtPrj.GetN(); i++)
	{
		if(YsShellBlend2CheckReach(p2,plVtPrj.GetN(),plVtPrj,i)==YSTRUE)
		{
			splitterIdx[0]=i;
			baseIdx=i+plVtPrj.GetN()/2;
			break;
		}
	}
	int testIdx;
	testIdx=baseIdx;
	for(i=0; i<plVtPrj.GetN(); i++)
	{
		testIdx+=(i%2==0 ? i : -i);
		testIdx=(testIdx+plVtPrj.GetN())%plVtPrj.GetN();
		if(testIdx!=splitterIdx[0] &&
		   YsShellBlend2CheckReach(p2,plVtPrj.GetN(),plVtPrj,testIdx)==YSTRUE)
		{
			splitterIdx[1]=testIdx;
			break;
		}
	} */



	if(splitterIdx[0]<0 || splitterIdx[1]<0)
	{
		YsErrOut("Cannot split the polygon.\n");
		return YSERR;
	}



	// (2) Append new polygons to cutterPlHdList
	int nOrgPlVt;
	const YsShellVertexHandle *orgPlVtHd;
	YsArray <YsShellVertexHandle,64> plVtHdList[2];

	nOrgPlVt=newShell.GetNumVertexOfPolygon(cutterPlHd);
	orgPlVtHd=newShell.GetVertexListOfPolygon(cutterPlHd);
	for(i=0; i<nOrgPlVt; i++)
	{
		if(i<splitterIdx[0] || splitterIdx[1]<i)
		{
			plVtHdList[0].Append(orgPlVtHd[i]);
		}
		else if(splitterIdx[0]<i && i<splitterIdx[1])
		{
			plVtHdList[1].Append(orgPlVtHd[i]);
		}
		else if(i==splitterIdx[0])
		{
			plVtHdList[0].Append(orgPlVtHd[splitterIdx[0]]);
			plVtHdList[0].Append(vtHd);
			plVtHdList[0].Append(orgPlVtHd[splitterIdx[1]]);
			plVtHdList[1].Append(orgPlVtHd[splitterIdx[0]]);
		}
		else if(i==splitterIdx[1])
		{
			plVtHdList[1].Append(orgPlVtHd[splitterIdx[1]]);
			plVtHdList[1].Append(vtHd);
		}
	}

	YsShellPolygon *orgCutterPlg,*newCutterPlg;
	orgCutterPlg=newShell.GetPolygon(cutterPlHd);


	ltc.DeletePolygon(cutterPlHd);

	YsShellPolygonHandle newPlHd[2];
	newPlHd[0]=newShell.AddPolygonH(plVtHdList[0].GetN(),plVtHdList[0]);
	newPlHd[1]=newShell.AddPolygonH(plVtHdList[1].GetN(),plVtHdList[1]);


	//if(newShell.GetSearchKey(newPlHd[0])==99 ||
	//   newShell.GetSearchKey(newPlHd[0])==90)
	//{
	//	YsPrintf("#### Watch! (1)AddPolygonH %d\n",newShell.GetSearchKey(newPlHd[0]));
	//}
	//if(newShell.GetSearchKey(newPlHd[1])==99 ||
	//   newShell.GetSearchKey(newPlHd[1])==90)
	//{
	//	YsPrintf("#### Watch! (2)AddPolygonH %d\n",newShell.GetSearchKey(newPlHd[1]));
	//}


	for(i=0; i<2; i++)
	{
		YsColor col;
		YsVec3 nom;

		newCutterPlg=newShell.GetPolygon(newPlHd[i]);
		newCutterPlg->freeAttribute0=orgCutterPlg->freeAttribute0; // 2002/12/24
		newCutterPlg->freeAttribute1=orgCutterPlg->freeAttribute1; // 2002/12/24
		newCutterPlg->freeAttribute2=orgCutterPlg->freeAttribute2;
		newCutterPlg->freeAttribute3=orgCutterPlg->freeAttribute3;
		newCutterPlg->freeAttribute4=orgCutterPlg->freeAttribute4;

		newShell.GetColorOfPolygon(col,cutterPlHd);
		newShell.GetNormalOfPolygon(nom,cutterPlHd);
		newShell.SetColorOfPolygon(newPlHd[i],col);
		newShell.SetNormalOfPolygon(newPlHd[i],nom);
	}

	ltc.AddPolygon(newPlHd[0]);
	ltc.AddPolygon(newPlHd[1]);

	cutterPlKeyList.Append(newShell.GetSearchKey(newPlHd[0]));
	cutterPlKeyList.Append(newShell.GetSearchKey(newPlHd[1]));

	newShell.DeletePolygon(cutterPlHd);

	createdPlHd.Set(2,newPlHd);

	return YSOK;
}

static YSRESULT YsShellBlend2ReconnectEdgeToEliminateSharpCorner
    (YsShell &newShell,	YsArray <YsShellPolygonHandle,16> &createdPlHd,YsShellLattice &ltc,const double &cosThr)
{
	const YsShellSearchTable *search;
	YsArray <unsigned,16> createdPlKey;
	int i;

	for(i=0; i<createdPlHd.GetN(); i++)
	{
		createdPlKey.Append(newShell.GetSearchKey(createdPlHd[i]));
	}

	search=newShell.GetSearchTable();

	for(i=0; i<createdPlKey.GetN(); i++)
	{
		YsShellPolygonHandle plHd;
		plHd=search->FindPolygon(newShell,createdPlKey[i]);
		if(plHd!=NULL)
		{
			YsShellBlend2ReconnectEdgeToEliminateSharpCorner(newShell,plHd,ltc,cosThr);
		}
	}

	return YSOK;
}

static YSRESULT YsShellBlend2ReconnectEdgeToEliminateSharpCorner
    (YsShell &newShell,YsShellPolygonHandle plHd,YsShellLattice &,const double &cosThr)
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

RETRY:
	nPlVt=newShell.GetNumVertexOfPolygon(plHd);
	plVtHd=newShell.GetVertexListOfPolygon(plHd);

	int i;
	for(i=0; i<nPlVt; i++)
	{
		YsVec3 p[3],v[2];
		YsShellVertexHandle cornerVtHd[3];

		cornerVtHd[0]=plVtHd[(i+nPlVt-1)%nPlVt];
		cornerVtHd[1]=plVtHd[i];
		cornerVtHd[2]=plVtHd[(i+1)%nPlVt];

		newShell.GetVertexPosition(p[0],cornerVtHd[0]);
		newShell.GetVertexPosition(p[1],cornerVtHd[1]);
		newShell.GetVertexPosition(p[2],cornerVtHd[2]);

		v[0]=p[1]-p[0];
		v[1]=p[1]-p[2];

		if(v[0].Normalize()==YSOK && v[1].Normalize()==YSOK && v[0]*v[1]>=cosThr)
		{
			// Corner at plVtHd[i] is a sharp corner.
			double refCos;
			int j;

			refCos=v[0]*v[1];

			for(j=0; j<2; j++)
			{
				if(YsShellBlend2EliminateSharpCornerByReconnectingEdge
				      (newShell,plHd,cornerVtHd[j],cornerVtHd[j+1],refCos,cosThr)==YSOK)
				{
					goto RETRY;
				}
			}
		}
	}
	return YSOK;
}

static YSRESULT YsShellBlend2EliminateSharpCornerByReconnectingEdge
     (YsShell &newShell,YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,
      const double &refCos,const double &/*cosThr*/)
{
	int i;
	const YsShellSearchTable *search;
	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;
	YsShellPolygonHandle neiPlHd;
	YsShellPolygon *plg[2];

	plg[0]=newShell.GetPolygon(plHd);

	neiPlHd=NULL;
	search=newShell.GetSearchTable();
	if(search->FindPolygonListByEdge(nFoundPlHd,foundPlHd,newShell,edVtHd0,edVtHd1)==YSOK && nFoundPlHd>0)
	{
		for(i=0; i<nFoundPlHd; i++)
		{
			plg[1]=newShell.GetPolygon(foundPlHd[i]);
			if(plHd!=foundPlHd[i] &&
			   plg[0]->freeAttribute2==plg[1]->freeAttribute2 &&
			   plg[0]->freeAttribute3==plg[1]->freeAttribute3)
			{
				neiPlHd=foundPlHd[i];
				break;
			}
		}
	}

	if(neiPlHd!=NULL)
	{
		// Is one of them a triangle?
		/*
		for(i=0; i<2; i++)
		{
			YsShellPolygonHandle triPlHd,nonTriPlHd;
			if(i==0)
			{
				triPlHd=plHd;
				nonTriPlHd=neiPlHd;
			}
			else
			{
				triPlHd=neiPlHd;
				nonTriPlHd=plHd;
			}
			if(newShell.GetNumVertexOfPolygon(triPlHd)==3)
			{
				YsShellVertexHandle midVtHd;
				const YsShellVertexHandle *plVtHd;
				plVtHd=newShell.GetVertexListOfPolygon(triPlHd);
				int j;
				midVtHd=NULL;
				for(j=0; j<3; j++)
				{
					if(plVtHd[j]!=edVtHd0 && plVtHd[j]!=edVtHd1)
					{
						midVtHd=plVtHd[j];
						break;
					}
				}
				if(midVtHd!=NULL)
				{
					int nPlVt;
					YsArray <YsShellVertexHandle,16> newPlVtHd;
					newPlVtHd.Set(0,NULL);
					nPlVt=newShell.GetNumVertexOfPolygon(nonTriPlHd);
					plVtHd=newShell.GetVertexListOfPolygon(nonTriPlHd);

					for(j=0; j<nPlVt; j++)
					{
						newPlVtHd.Append(plVtHd[j]);
						if((plVtHd[j]==edVtHd0 && plVtHd[(j+1)%nPlVt]==edVtHd1) ||
						   (plVtHd[j]==edVtHd1 && plVtHd[(j+1)%nPlVt]==edVtHd0))
						{
							newPlVtHd.Append(midVtHd);
						}
					}
					if(newShell.CheckPotentialPolygon(newPlVtHd.GetN(),newPlVtHd,refCos)==YSOK)
					{
						newShell.ModifyPolygon(plHd,newPlVtHd.GetN(),newPlVtHd);
						newShell.DeletePolygon(neiPlHd);
						YsPrintf("Merged: Triangle\n");
						return YSOK;
					}
				}
			}
		}
		*/


		// If none of the two polygons is a triangle.
		for(i=0; i<2; i++)
		{
			YsShellVertexHandle edVtHd[2];
			edVtHd[i  ]=edVtHd0;
			edVtHd[1-i]=edVtHd1;

			int nNeiPlVt;
			const YsShellVertexHandle *neiPlVtHd;
			YsArray <YsVec3> neiPlVtPos;
			nNeiPlVt=newShell.GetNumVertexOfPolygon(neiPlHd);
			neiPlVtHd=newShell.GetVertexListOfPolygon(neiPlHd);
			newShell.GetVertexListOfPolygon(neiPlVtPos,neiPlHd);

			int j,pvtIdx;
			pvtIdx=-1;
			for(j=0; j<nNeiPlVt; j++)
			{
				if(neiPlVtHd[j]==edVtHd[0])
				{
					pvtIdx=j;
					break;
				}
			}

			if(pvtIdx==-1)
			{
				YsErrOut("YsShellBlend2EliminateSharpCornerByReconnectingEdge()\n");
				YsErrOut("  Warning: Shell search table is broken.\n");
				return YSERR;
			}

			for(j=0; j<nNeiPlVt; j++)
			{
				if(YsAbs(j-pvtIdx)>1 && (j!=0 || pvtIdx!=nNeiPlVt-1) && (j!=nNeiPlVt-1 || pvtIdx!=0))
				{
					if(YsCheckSeparatability3(neiPlVtPos.GetN(),neiPlVtPos,pvtIdx,j)==YSTRUE)
					{
						YsVec3 p[3],v[2];
						p[0]=neiPlVtPos[pvtIdx];
						p[1]=neiPlVtPos[j];
						p[2]=neiPlVtPos[(j+1)%nNeiPlVt];
						v[0]=p[1]-p[0];
						v[1]=p[1]-p[2];
						if(v[0].Normalize()!=YSOK || v[1].Normalize()!=YSOK || v[0]*v[1]>=refCos)
						{
							goto NEXTJ;
						}
						p[2]=neiPlVtPos[(j+nNeiPlVt-1)%nNeiPlVt];
						v[0]=p[1]-p[0];
						v[1]=p[1]-p[2];
						if(v[0].Normalize()!=YSOK || v[1].Normalize()!=YSOK || v[0]*v[1]>=refCos)
						{
							goto NEXTJ;
						}

						// Split at j and pvtIdx
						YsArray <YsShellVertexHandle,16> newPlVtHd[2];
						int m,split[2],toMerge;
						newPlVtHd[0].Set(0,NULL);
						newPlVtHd[1].Set(0,NULL);
						split[0]=YsSmaller(j,pvtIdx);
						split[1]=YsGreater(j,pvtIdx);
						toMerge=-1;
						for(m=0; m<=split[0]; m++)
						{
							newPlVtHd[0].Append(neiPlVtHd[m]);
							if(neiPlVtHd[m]==edVtHd[1])
							{
								toMerge=0;
							}
						}
						for(m=split[0]; m<=split[1]; m++)
						{
							newPlVtHd[1].Append(neiPlVtHd[m]);
							if(neiPlVtHd[m]==edVtHd[1])
							{
								toMerge=1;
							}
						}
						for(m=split[1]; m<nNeiPlVt; m++)
						{
							newPlVtHd[0].Append(neiPlVtHd[m]);
							if(neiPlVtHd[m]==edVtHd[1])
							{
								toMerge=0;
							}
						}

						if(toMerge>=0)
						{
							int i;
							YSBOOL found;
							found=YSFALSE;
							for(i=0; i<newPlVtHd[toMerge].GetN(); i++)
							{
								if((newPlVtHd[toMerge].GetTopItem()==edVtHd[0] &&
								    newPlVtHd[toMerge].GetEndItem()==edVtHd[1]) ||
								   (newPlVtHd[toMerge].GetTopItem()==edVtHd[1] &&
								    newPlVtHd[toMerge].GetEndItem()==edVtHd[0]))
								{
									found=YSTRUE;
									break;
								}
								YsShellVertexHandle a;
								a=newPlVtHd[toMerge].GetTopItem();
								newPlVtHd[toMerge].Delete(0);
								newPlVtHd[toMerge].Append(a);
							}

							if(found==YSTRUE)
							{
								int nPlVt;
								const YsShellVertexHandle *plVtHd;
								YsArray <YsShellVertexHandle> merge;

								nPlVt=newShell.GetNumVertexOfPolygon(plHd);
								plVtHd=newShell.GetVertexListOfPolygon(plHd);

								for(i=0; i<nPlVt; i++)
								{
									merge.Append(plVtHd[i]);
									if(plVtHd[i]==newPlVtHd[toMerge].GetTopItem())
									{
										YSSIZE_T j;
										for(j=1; j<newPlVtHd[toMerge].GetN(); j++)
										{
											merge.Append(newPlVtHd[toMerge][j]);
										}
										i++;
									}
									else if(plVtHd[i]==newPlVtHd[toMerge].GetEndItem())
									{
										YSSIZE_T j;
										for(j=newPlVtHd[toMerge].GetN()-2; j>=0; j--)
										{
											merge.Append(newPlVtHd[toMerge][j]);
										}
										i++;
									}
								}

								// New Polygons
								//    plHd -> merge
								//    neiPlHd -> newPlVtHd[1-toMerge]

								YSBOOL dblVertex;  // Don't allow a polygon to use a vertex more than once.
								int m,n;
								dblVertex=YSFALSE;
								for(m=0; m<merge.GetN(); m++)
								{
									for(n=m+1; n<merge.GetN(); n++)
									{
										if(merge[m]==merge[n])
										{
											dblVertex=YSTRUE;
											break;
										}
									}
								}
								for(m=0; m<newPlVtHd[1-toMerge].GetN(); m++)
								{
									for(n=m+1; n<newPlVtHd[1-toMerge].GetN(); n++)
									{
										if(newPlVtHd[1-toMerge][m]==newPlVtHd[1-toMerge][n])
										{
											dblVertex=YSTRUE;
											break;
										}
									}
								}

								if(dblVertex!=YSTRUE &&
								   newShell.CheckPotentialPolygon
								       (merge.GetN(),merge,refCos)==YSOK &&
								   newShell.CheckPotentialPolygon
								       (newPlVtHd[1-toMerge].GetN(),newPlVtHd[1-toMerge],refCos)==YSOK)
								{
									newShell.ModifyPolygon(plHd,merge.GetN(),merge);
									newShell.ModifyPolygon(neiPlHd,newPlVtHd[1-toMerge].GetN(),newPlVtHd[1-toMerge]);
									YsPrintf("\n Merged: Non-Triangle %d %d\n",
									   newShell.GetSearchKey(plHd),
									   newShell.GetSearchKey(neiPlHd));
									return YSOK;
								}
							}
							else
							{
								YsErrOut("YsShellBlend2EliminateSharpCornerByReconnectingEdge()\n");
								YsErrOut("  Warning: Cannot re-order the polygon vertex list.\n");
								return YSERR;
							}
						}
						else
						{
							YsErrOut("YsShellBlend2EliminateSharpCornerByReconnectingEdge()\n");
							YsErrOut("  Warning: Cannot decide toMerge.\n");
							return YSERR;
						}
					}
				}
			NEXTJ:
				;
			}
		}
	}

	return YSERR;
}

YSRESULT YsShellBlend2InsertVertexToEdge
	(YsShell &newShell,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2,YsShellVertexHandle newVtHd,
	 YsShellLattice &ltc)
{
	if(edVtHd1==newVtHd || edVtHd2==newVtHd)
	{
		return YSOK;
	}


	const YsShellSearchTable *search;
	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	search=newShell.GetSearchTable();
	if(search->FindPolygonListByEdge(nFoundPlHd,foundPlHd,newShell,edVtHd1,edVtHd2)==YSOK && nFoundPlHd>0)
	{
		YsArray <YsShellPolygonHandle,2> plHdList;
		YsArray <YsShellVertexHandle> vtHdList;
		plHdList.Set(nFoundPlHd,foundPlHd);

		int i;
		for(i=0; i<plHdList.GetN(); i++)
		{
			ltc.DeletePolygon(plHdList[i]);

			newShell.GetVertexListOfPolygon(vtHdList,plHdList[i]);
			if(YSTRUE!=vtHdList.IsIncluded(newVtHd))
			{
				for(int j=0; j<vtHdList.GetN(); j++)
				{
					if((vtHdList[j]==edVtHd1 && vtHdList[(j+1)%vtHdList.GetN()]==edVtHd2) ||
					   (vtHdList[j]==edVtHd2 && vtHdList[(j+1)%vtHdList.GetN()]==edVtHd1))
					{
						vtHdList.Insert(j+1,newVtHd);
					}
				}
				newShell.ModifyPolygon(plHdList[i],vtHdList.GetN(),vtHdList);
				ltc.AddPolygon(plHdList[i]);
			}
			else
			{
				// 2016/04/23 The vertex to be inserted is already used by this polygon.
				//            The polygon is split into two polygons, or the "appendix" should be removed.
				int insertionIdx=-1;
				for(int j=0; j<vtHdList.GetN(); j++)
				{
					if((vtHdList[j]==edVtHd1 && vtHdList[(j+1)%vtHdList.GetN()]==edVtHd2) ||
					   (vtHdList[j]==edVtHd2 && vtHdList[(j+1)%vtHdList.GetN()]==edVtHd1))
					{
						vtHdList.Insert(j+1,newVtHd);
						insertionIdx=j+1;
					}
				}

				YsArray <YsShell::VertexHandle> newPlVtHd[2];
				int state=1;
				for(int j=0; j<vtHdList.GetN(); ++j)
				{
					if(newVtHd==vtHdList.GetCyclic(insertionIdx+j))
					{
						state=1-state;
					}
					newPlVtHd[state].Add(vtHdList.GetCyclic(insertionIdx+j));
				}
				if(3<=newPlVtHd[0].GetN() && 3<=newPlVtHd[1].GetN())
				{
					newShell.ModifyPolygon(plHdList[i],newPlVtHd[0]);
					ltc.AddPolygon(plHdList[i]);

					auto newPlHd=newShell.AddPolygonH(newPlVtHd[1].GetN(),newPlVtHd[1]);
					newShell.GetPolygon(newPlHd)->freeAttribute0=newShell.GetPolygon(plHdList[i])->freeAttribute0;
					newShell.GetPolygon(newPlHd)->freeAttribute1=newShell.GetPolygon(plHdList[i])->freeAttribute1;
					newShell.GetPolygon(newPlHd)->freeAttribute2=newShell.GetPolygon(plHdList[i])->freeAttribute2;
					newShell.GetPolygon(newPlHd)->freeAttribute3=newShell.GetPolygon(plHdList[i])->freeAttribute3;
					newShell.GetPolygon(newPlHd)->freeAttribute4=newShell.GetPolygon(plHdList[i])->freeAttribute4;
				}
				else if(3<=newPlVtHd[0].GetN())
				{
					newShell.ModifyPolygon(plHdList[i],newPlVtHd[0]);
					ltc.AddPolygon(plHdList[i]);
				}
				else if(3<=newPlVtHd[1].GetN())
				{
					newShell.ModifyPolygon(plHdList[i],newPlVtHd[1]);
					ltc.AddPolygon(plHdList[i]);
				}
			}
		}
	}

	return YSOK;
}

static YSRESULT YsBlendShell2EdgeEdgeIntersection(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp)
{
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle> plVtHdList;
	YsLoopCounter ctr;


	#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
	#endif


	YsPrintf("Edge-Edge Intersection.\n");
	ctr.BeginCounter(newShell.GetNumPolygon());

	plHd=NULL;
	while((plHd=newShell.FindNextPolygon(plHd))!=NULL)
	{
		ctr.Increment();

		int i;
		int nPlVtHd;
		const YsShellVertexHandle *plVtHd;
		const YsShellPolygon *plg;

		nPlVtHd=newShell.GetNumVertexOfPolygon(plHd);
		plVtHd=newShell.GetVertexListOfPolygon(plHd);
		plg=newShell.GetPolygon(plHd);

		plVtHdList.Set(nPlVtHd,plVtHd);
		for(i=0; i<plVtHdList.GetN(); i++)
		{
			YsShellVertexHandle edVtHd[2];
			edVtHd[0]=plVtHdList[i];
			edVtHd[1]=plVtHdList[(i+1)%plVtHdList.GetN()];
			YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,edVtHd[0],edVtHd[1],plg->freeAttribute2,singleShellOp);
		}
	}

	ctr.EndCounter();

	return YSOK;
}

static YSRESULT YsBlendShell2EdgeEdgeIntersectionForOneEdge
    (YsShell &newShell,YsShellLattice &ltc,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2,int edgeShlId,YSBOOL singleShellOp)
{
	YsVec3 edVtPos[2];
	YsArray <YsShellPolygonHandle,16> collidePlHd;

	newShell.GetVertexPosition(edVtPos[0],edVtHd1);
	newShell.GetVertexPosition(edVtPos[1],edVtHd2);

	if(ltc.MakePossibllyCollidingPolygonList(collidePlHd,2,edVtPos)==YSOK)
	{
		int i;
		for(i=0; i<collidePlHd.GetN(); i++)
		{
			int nPlVtHd;
			const YsShellVertexHandle *plVtHd;
			const YsShellPolygon *plg;

			nPlVtHd=newShell.GetNumVertexOfPolygon(collidePlHd[i]);
			plVtHd=newShell.GetVertexListOfPolygon(collidePlHd[i]);
			plg=newShell.GetPolygon(collidePlHd[i]);

			if(plg->freeAttribute2!=edgeShlId || YSTRUE==singleShellOp)
			{
				int j;
				for(j=0; j<nPlVtHd; j++)
				{
					YsShellVertexHandle opEdVtHd[2];
					opEdVtHd[0]=plVtHd[j];
					opEdVtHd[1]=plVtHd[(j+1)%nPlVtHd];

					if(opEdVtHd[0]!=edVtHd1 && opEdVtHd[0]!=edVtHd2 &&
					   opEdVtHd[1]!=edVtHd1 && opEdVtHd[1]!=edVtHd2)
					{
						YsVec3 opEdVtPos[2];
						newShell.GetVertexPosition(opEdVtPos[0],opEdVtHd[0]);
						newShell.GetVertexPosition(opEdVtPos[1],opEdVtHd[1]);

						if(opEdVtPos[0]==edVtPos[0])
						{
							YsShellBlend2SubstVtHd(newShell,opEdVtHd[0],edVtHd1);
						}
						else if(opEdVtPos[0]==edVtPos[1])
						{
							YsShellBlend2SubstVtHd(newShell,opEdVtHd[0],edVtHd2);
						}
						else if(opEdVtPos[1]==edVtPos[0])
						{
							YsShellBlend2SubstVtHd(newShell,opEdVtHd[1],edVtHd1);
						}
						else if(opEdVtPos[1]==edVtPos[1])
						{
							YsShellBlend2SubstVtHd(newShell,opEdVtHd[1],edVtHd2);
						}
						else
						{
							YsVec3 nea1,nea2;
							YsGetNearestPointOfTwoLine(nea1,nea2,edVtPos[0],edVtPos[1],opEdVtPos[0],opEdVtPos[1]);
							if(nea1==nea2 &&
							   YsCheckInBetween3(nea1,edVtPos[0],edVtPos[1])==YSTRUE &&
							   YsCheckInBetween3(nea2,opEdVtPos[0],opEdVtPos[1])==YSTRUE)
							{
								#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
								YSBOOL watchMatch=YSFALSE;
								{
									for(auto &watchPos : watchItscPos)
									{
										if(watchPos==nea1)
										{
											printf("Near1 matches! (%s)-(%s)\n",nea1.Txt(),watchPos.Txt());
											watchMatch=YSTRUE;
										}
										if(watchPos==nea2)
										{
											printf("Near2 matches! (%s)-(%s)\n",nea2.Txt(),watchPos.Txt());
											watchMatch=YSTRUE;
										}
									}
									if(YSTRUE==watchMatch)
									{
										printf("edVtPos (%s)\n",edVtPos[0].Txt());
										printf("edVtPos (%s)\n",edVtPos[1].Txt());
										printf("opEdVtPos (%s)\n",opEdVtPos[0].Txt());
										printf("opEdVtPos (%s)\n",opEdVtPos[1].Txt());
									}
								}
								#endif

								if(nea1==edVtPos[0])
								{
									if(YSOK==YsShellBlend2CheckEdgeVertexInsertionCheck(newShell,opEdVtHd[0],opEdVtHd[1],edVtHd1))
									{
										YsShellBlend2InsertVertexToEdge(newShell,opEdVtHd[0],opEdVtHd[1],edVtHd1,ltc);
										#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
										if(YSTRUE==watchMatch)
										{
											printf("%s %d>",__FUNCTION__,__LINE__);getchar();
										}
										#endif
										return YSOK;
									}
									return YSERR;
								}
								else if(nea1==edVtPos[1])
								{
									if(YSOK==YsShellBlend2CheckEdgeVertexInsertionCheck(newShell,opEdVtHd[0],opEdVtHd[1],edVtHd2))
									{
										YsShellBlend2InsertVertexToEdge(newShell,opEdVtHd[0],opEdVtHd[1],edVtHd2,ltc);
										#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
										if(YSTRUE==watchMatch)
										{
											printf("%s %d>",__FUNCTION__,__LINE__);getchar();
										}
										#endif
										return YSOK;
									}
									return YSERR;
								}
								else if(nea2==opEdVtPos[0])
								{
									if(YSOK==YsShellBlend2CheckEdgeVertexInsertionCheck(newShell,edVtHd1,edVtHd2,opEdVtHd[0]))
									{
										YsShellBlend2InsertVertexToEdge(newShell,edVtHd1,edVtHd2,opEdVtHd[0],ltc);
										YSRESULT res1,res2;
										res1=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,edVtHd1,opEdVtHd[0],edgeShlId,singleShellOp);
										res2=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,opEdVtHd[0],edVtHd2,edgeShlId,singleShellOp);
										#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
										if(YSTRUE==watchMatch)
										{
											printf("%s %d>",__FUNCTION__,__LINE__);getchar();
										}
										#endif
										return ((res1==YSOK && res2==YSOK) ? YSOK : YSERR);
									}
									return YSERR;
								}
								else if(nea2==opEdVtPos[1])
								{
									if(YSOK==YsShellBlend2CheckEdgeVertexInsertionCheck(newShell,edVtHd1,edVtHd2,opEdVtHd[1]))
									{
										YsShellBlend2InsertVertexToEdge(newShell,edVtHd1,edVtHd2,opEdVtHd[1],ltc);
										YSRESULT res1,res2;
										res1=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,edVtHd1,opEdVtHd[1],edgeShlId,singleShellOp);
										res2=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,opEdVtHd[1],edVtHd2,edgeShlId,singleShellOp);
										#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
										if(YSTRUE==watchMatch)
										{
											printf("%s %d>",__FUNCTION__,__LINE__);getchar();
										}
										#endif
										return ((res1==YSOK && res2==YSOK) ? YSOK : YSERR);
									}
									return YSERR;
								}
								else
								{
									YsShellVertexHandle newVtHd;
									newVtHd=newShell.AddVertexH((nea1+nea2)/2.0);
									newShell.GetVertex(newVtHd)->freeAttribute3=-1;
									newShell.GetVertex(newVtHd)->freeAttribute4=-1;
									YsShellBlend2InsertVertexToEdge(newShell,edVtHd1,edVtHd2,newVtHd,ltc);
									YsShellBlend2InsertVertexToEdge(newShell,opEdVtHd[0],opEdVtHd[1],newVtHd,ltc);

									YSRESULT res1,res2;
									res1=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,edVtHd1,newVtHd,edgeShlId,singleShellOp);
									res2=YsBlendShell2EdgeEdgeIntersectionForOneEdge(newShell,ltc,newVtHd,edVtHd2,edgeShlId,singleShellOp);
									#ifdef YSDEBUG_EDGE_EDGE_INTERSECTION
									if(YSTRUE==watchMatch)
									{
										printf("%s %d>",__FUNCTION__,__LINE__);getchar();
									}
									#endif
									return ((res1==YSOK && res2==YSOK) ? YSOK : YSERR);
								}
							}
						}
					}
				}
			}
		}
	}
	return YSOK;
}

static YSRESULT YsShellBlend2CheckEdgeVertexInsertionCheck(YsShell &newShell,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YsShellVertexHandle insVtHd)
{
	return YSOK;

// This check didn't save 20141205-boolerror0003 after all.
//	// See 20141205-boolerror0003
//	const auto search=newShell.GetSearchTable();
//	if(0<search->GetNumPolygonUsingEdge(newShell,edVtHd0,insVtHd) ||
//	   0<search->GetNumPolygonUsingEdge(newShell,edVtHd1,insVtHd))  // The new edge already exists.
//	{
//		int nEdPlHd;
//		const YsShellPolygonHandle *edPlHd;
//		if(YSOK==search->FindPolygonListByEdge(nEdPlHd,edPlHd,newShell,edVtHd0,edVtHd1) && 0<nEdPlHd)
//		{
//			for(int idx0=0; idx0<nEdPlHd; ++idx0)
//			{
//				int nPlVt;
//				const YsShellVertexHandle *plVtHd;
//				newShell.GetPolygon(nPlVt,plVtHd,edPlHd[idx0]);
//				if(YSTRUE==YsIsIncluded(nPlVt,plVtHd,insVtHd))
//				{
//					return YSERR;
//				}
//			}
//		}
//	}
//	return YSOK;
}

static YSRESULT YsShellBlend2SubstVtHd(YsShell &newShell,YsShellVertexHandle changeThis,YsShellVertexHandle intoThis)
{
	const YsShellSearchTable *search;
	YsArray <YsShellPolygonHandle> plHdList;
	YsArray <YsShellVertexHandle> plVtHdList;

	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	search=newShell.GetSearchTable();
	if(search->FindPolygonListByVertex(nFoundPlHd,foundPlHd,newShell,changeThis)==YSOK && nFoundPlHd>0)
	{
		int i,j;
		plHdList.Set(nFoundPlHd,foundPlHd);

		for(i=0; i<plHdList.GetN(); i++)
		{
			newShell.GetVertexListOfPolygon(plVtHdList,plHdList[i]);
			for(j=0; j<plVtHdList.GetN(); j++)
			{
				if(plVtHdList[j]==changeThis)
				{
					plVtHdList[j]=intoThis;
				}
			}
			newShell.ModifyPolygon(plHdList[i],plVtHdList.GetN(),plVtHdList);
		}
	}
	return YSOK;
}

static YSBOOL YsShellBlend2CheckReach(const YsVec2 &pvt,YSSIZE_T np,const YsVec2 p[],int idx)
{
	YsVec2 mid,tst1,tst2;
	mid=(p[idx]+pvt)/2.0;
	if(YsCheckInsidePolygon2(mid,np,p)==YSINSIDE)
	{
		int i;
		for(i=0; i<np; i++)
		{
			if(i!=idx && ((i+1)%np)!=idx)
			{
				tst1=p[i];
				tst2=p[(i+1)%np];

				if(YsGetLinePenetration2(p[idx],pvt,tst1,tst2)!=YSAPART)
				{
					return YSFALSE;
				}
			}
		}
		return YSTRUE;
	}
	return YSFALSE;
}

static YSRESULT YsBlendShell2SlashFaceByFace(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp)
{
	YsShellPolygonHandle plHd;
	YsShellPolygon *toBeSlashed,*cutter;
	YsArray <YsVec3> plVtPos;
	YsArray <YsShellPolygonHandle,16> collidePlHd;
	int nPlVtHd;
	YsVec3 nom;
	YSBOOL watch;

	YsArray <YsArray <YsVec2,16> > plVtPosList;
	YsArray <YsArray <YsShellVertexHandle,16> > plVtHdList;
	YsArray <YsShellPolygonHandle> toBeDeleted;

	plHd=NULL;
	while((plHd=newShell.FindNextPolygon(plHd))!=NULL)
	{
		toBeSlashed=newShell.GetPolygon(plHd);
		nPlVtHd=newShell.GetNumVertexOfPolygon(plHd);

		watch=YSFALSE;
		// if(0)
		// {
		// 	watch=YSTRUE;
		// 	YsPrintf("Watch!  nPlVtHd=%d key=%d\n",nPlVtHd,newShell.GetSearchKey(plHd));
		// }

		newShell.GetVertexListOfPolygon(plVtPos,plHd);
		if(YsBlendShell2GetNormalOfPolygon(nom,newShell,plHd)!=YSOK)
		{
			YsErrOut("\n");
			YsErrOut("Cannot compute a normal for a polygon.  Skipping it.\n");
			goto NEXTPOLYGON;
		}

		if(ltc.MakePossibllyCollidingPolygonList(collidePlHd,plVtPos.GetN(),plVtPos)==YSOK &&
		   collidePlHd.GetN()>1)
		{
			int i;

			YsAtt3 att;
			YsVec2 prj;
			YsArray <YsVec2,16> plVtPos2;
			YsArray <YsShellVertexHandle,16> plVtHd;
			att.SetForwardVector(nom);

			for(i=0; i<nPlVtHd; i++)
			{
				YsVec3 x;
				att.MulInverse(x,plVtPos[i]);
				prj.Set(x.x(),x.y());
				plVtPos2.Append(prj);
			}

			plVtHd.Set(newShell.GetNumVertexOfPolygon(plHd),newShell.GetVertexListOfPolygon(plHd));

			plVtPosList.Set(0,NULL);
			plVtPosList.Append(plVtPos2);
			plVtHdList.Set(0,NULL);
			plVtHdList.Append(plVtHd);

			for(i=0; i<collidePlHd.GetN(); i++)
			{
				cutter=newShell.GetPolygon(collidePlHd[i]);
				if(cutter->freeAttribute2!=toBeSlashed->freeAttribute2 || (YSTRUE==singleShellOp && plHd!=collidePlHd[i]))
				{
					int j,nCutPlVtHd;
					YsArray <YsVec3,16> cutterPlVtPos;
					YsArray <YsVec2,16> cutterPlVtPos2;
					const YsShellVertexHandle *cutPlVtHd;
					YsVec3 cutterNom;
					YsAtt3 prj;

					if(watch==YSTRUE)
					{
						YsPrintf("Cutter key=%d\n",newShell.GetSearchKey(collidePlHd[i]));
					}

					nCutPlVtHd=newShell.GetNumVertexOfPolygon(collidePlHd[i]);
					cutPlVtHd=newShell.GetVertexListOfPolygon(collidePlHd[i]);

					cutterPlVtPos.Set(nCutPlVtHd,NULL);
					for(j=0; j<nCutPlVtHd; j++)
					{
						newShell.GetVertexPosition(cutterPlVtPos[j],cutPlVtHd[j]);
					}

					newShell.GetNormalOfPolygon(cutterNom,collidePlHd[i]);
					if(cutterNom==YsOrigin())
					{
						if(YsGetAverageNormalVector(cutterNom,cutterPlVtPos.GetN(),cutterPlVtPos)!=YSOK)
						{
							YsErrOut("Warning: Cannot compute a normal for a polygon.  Skipping it.\n");
							cutterNom=YsZVec();
						}
					}

					prj.SetForwardVector(cutterNom);
					cutterPlVtPos2.Set(nCutPlVtHd,NULL);
					for(j=0; j<nCutPlVtHd; j++)
					{
						prj.MulInverse(cutterPlVtPos[j],cutterPlVtPos[j]);
						cutterPlVtPos2[j].Set(cutterPlVtPos[j].x(),cutterPlVtPos[j].y());
					}

					YsShellBlend2SlashPolygonSet(plVtHdList,plVtPosList,nCutPlVtHd,cutPlVtHd,cutterPlVtPos2,watch);

					if(watch==YSTRUE)
					{
						printf("Watch!  plVtHdList.GetN()=%d\n",(int)plVtHdList.GetN());
					}
				}
			}

			if(watch==YSTRUE)
			{
				printf("Watch!  plVtHdList.GetN()=%d in the end\n",(int)plVtHdList.GetN());
			}

			if(plVtHdList.GetN()>1)
			{
				YsVec3 nom;
				YsColor col;
				newShell.GetNormalOfPolygon(nom,plHd);
				newShell.GetColorOfPolygon(col,plHd);

				ltc.DeletePolygon(plHd);

				for(i=0; i<plVtHdList.GetN(); i++)
				{
					YsShellPolygonHandle newPlHd;
					YsShellPolygon *newPlg;
					newPlHd=newShell.AddPolygonH(plVtHdList[i].GetN(),plVtHdList[i]);


					//if(newShell.GetSearchKey(newPlHd)==99 ||
					//   newShell.GetSearchKey(newPlHd)==90)
					//{
					//	YsPrintf("#### Watch! (3)AddPolygonH %d\n",newShell.GetSearchKey(newPlHd));
					//}


					newPlg=newShell.GetPolygon(newPlHd);
					newPlg->freeAttribute0=toBeSlashed->freeAttribute0; // 2002/12/24
					newPlg->freeAttribute1=toBeSlashed->freeAttribute1; // 2002/12/24
					newPlg->freeAttribute2=toBeSlashed->freeAttribute2;
					newPlg->freeAttribute3=toBeSlashed->freeAttribute3;
					newPlg->freeAttribute4=toBeSlashed->freeAttribute4;

					newShell.SetNormalOfPolygon(newPlHd,nom);
					newShell.SetColorOfPolygon(newPlHd,col);

					ltc.AddPolygon(newPlHd);
				}

				toBeDeleted.Append(plHd);
			}
		}

	NEXTPOLYGON:
		;
	}

	int i;
	for(i=0; i<toBeDeleted.GetN(); i++)
	{
		newShell.DeletePolygon(toBeDeleted[i]);
	}

	return YSOK;
}


static YSRESULT YsShellBlend2SlashPolygonSet
    (YsArray <YsArray <YsShellVertexHandle,16> > &plVtHdList,
     YsArray <YsArray <YsVec2,16> > &plVtPosList,
     int nCutPlVtHd,const YsShellVertexHandle cutPlVtHd[],const YsVec2 cutPlVtPos[],YSBOOL watch)
{
	YSSIZE_T i,j,k;
	YsArray <YSSIZE_T,16> toBeDeleted;
	YsArray <YsShellVertexHandle,16> sharedVtHd;
	YsArray <int,16> sharedVtIdOfPl;
	YsArray <int,16> sharedVtIdOfCut;

	YsArray <YsShellVertexHandle,16> newPlVtHd[2];
	YsArray <YsVec2,16> newPlVtPos[2];

	for(i=0; i<plVtHdList.GetN(); i++)
	{
		sharedVtHd.Set(0,NULL);
		sharedVtIdOfPl.Set(0,NULL);
		sharedVtIdOfCut.Set(0,NULL);
		for(j=0; j<plVtHdList[i].GetN(); j++)
		{
			for(k=0; k<nCutPlVtHd; k++)
			{
				if(cutPlVtHd[k]==plVtHdList[i][j])
				{
					sharedVtHd.Append(cutPlVtHd[k]);
					sharedVtIdOfPl.Append((int)j);
					sharedVtIdOfCut.Append((int)k);
				}
			}
		}

		for(j=0; j<sharedVtIdOfPl.GetN(); j++)
		{
			for(k=j+1; k<sharedVtIdOfPl.GetN(); k++)
			{
				YsVec2 cutterMid;
				cutterMid=(cutPlVtPos[sharedVtIdOfCut[j]]+cutPlVtPos[sharedVtIdOfCut[k]])/2.0;

				// >>  To avoid numerical errors problem in YsCheckInsidePolygon2
				YSBOOL isCutterEdge;
				if(YsAbs(sharedVtIdOfCut[j]-sharedVtIdOfCut[k])==1 ||
				   (sharedVtIdOfCut[j]==0 && sharedVtIdOfCut[k]==nCutPlVtHd-1) ||
				   (sharedVtIdOfCut[k]==0 && sharedVtIdOfCut[j]==nCutPlVtHd-1))
				{
					isCutterEdge=YSTRUE;
				}
				else
				{
					isCutterEdge=YSFALSE;
				}
				// <<


				if(watch==YSTRUE)
				{
					YSSIDE side;
					YSBOOL separatable;
					separatable=YsCheckSeparatability2
					    (plVtPosList[i].GetN(),plVtPosList[i],sharedVtIdOfPl[j],sharedVtIdOfPl[k]);
					side=YsCheckInsidePolygon2(cutterMid,nCutPlVtHd,cutPlVtPos);
					YsPrintf("%s\n",(separatable==YSTRUE ? "SEPARATABLE" : "NON_SEPARATABLE"));
					switch(side)
					{
					default:
						YsPrintf("UNKNOWNSIDE\n");
						break;
					case YSINSIDE:
						YsPrintf("INSIDE\n");
						break;
					case YSOUTSIDE:
						YsPrintf("OUTSIDE\n");
						break;
					case YSBOUNDARY:
						YsPrintf("BOUNDARY\n");
						break;
					}
					if(isCutterEdge==YSTRUE)
					{
						YsPrintf("Is Cutter Edge.\n");
					}
					else
					{
						YsPrintf("Is NOT Cutter Edge.\n");
					}
				}

				if(YsCheckSeparatability2
				    (plVtPosList[i].GetN(),plVtPosList[i],sharedVtIdOfPl[j],sharedVtIdOfPl[k])==YSTRUE &&
				   (isCutterEdge==YSTRUE || YsCheckInsidePolygon2(cutterMid,nCutPlVtHd,cutPlVtPos)!=YSOUTSIDE))
				{
					int m;

					newPlVtHd[0].Set(0,NULL);
					newPlVtHd[1].Set(0,NULL);
					newPlVtPos[0].Set(0,NULL);
					newPlVtPos[1].Set(0,NULL);

					for(m=0; m<=sharedVtIdOfPl[j]; m++)
					{
						newPlVtHd[0].Append(plVtHdList[i][m]);
						newPlVtPos[0].Append(plVtPosList[i][m]);
					}
					for(m=sharedVtIdOfPl[j]; m<=sharedVtIdOfPl[k]; m++)
					{
						newPlVtHd[1].Append(plVtHdList[i][m]);
						newPlVtPos[1].Append(plVtPosList[i][m]);
					}
					for(m=sharedVtIdOfPl[k]; m<plVtHdList[i].GetN(); m++)
					{
						newPlVtHd[0].Append(plVtHdList[i][m]);
						newPlVtPos[0].Append(plVtPosList[i][m]);
					}

					if(newPlVtHd[0].GetN()>2 && newPlVtHd[1].GetN()>2)
					{
						plVtHdList.Append(newPlVtHd[0]);
						plVtHdList.Append(newPlVtHd[1]);
						plVtPosList.Append(newPlVtPos[0]);
						plVtPosList.Append(newPlVtPos[1]);
						toBeDeleted.Append(i);
						goto NEXTPOLYGON;
					}
				}
			}
		}

	NEXTPOLYGON:
		;
	}

	for(i=toBeDeleted.GetN()-1; i>=0; i--)
	{
		plVtHdList.DeleteBySwapping(toBeDeleted[i]);
		plVtPosList.DeleteBySwapping(toBeDeleted[i]);
	}

	return YSOK;
}


////////////////////////////////////////////////////////////

// static YSRESULT YsBlendShell2KillCLoop(YsShell &/*newShell*/,YsShellLattice &/*ltc*/,const double &/*cosThr*/)
// {
// 	return YSERR;
// }

////////////////////////////////////////////////////////////

static YSRESULT YsBlendShell2OverlappingPolygon(YsShell &newShell,YsShellLattice &ltc,const double &cosThr,YSBOOL singleShellOp)
{
	YsShellPolygonHandle plHd,nextPlHd;
	YsArray <YsShellPolygonHandle,16> samePlnPlHd;
	YsArray <YsShellPolygonHandle,16> toBeDeleted;
	YsShellPolygon *plg;
	YsVec2 pos2;
	YsVec3 nom,prj;
	YsAtt3 att;
	YsMatrix4x4 prjMat;


	YsShellPolygonHandle problematicPlHd;
	problematicPlHd=NULL;


	YsArray <YsShellPolygonHandle> todo;
	YsHashTable <YsShellPolygonHandle> covered(65533);
	YsArray <YsShellPolygonHandle,16> createdPlHd;

	for(;;) // Visual C++ complains for "while(1)"
	{
		plHd=NULL;
		todo.Set(0,NULL);
		while((plHd=newShell.FindNextPolygon(plHd))!=NULL)
		{
			YsShellPolygonHandle done;
			if(covered.FindElement(done,newShell.GetSearchKey(plHd))!=YSOK)
			{
				todo.Append(plHd);
				covered.AddElement(newShell.GetSearchKey(plHd),plHd);
			}
		}

		if(todo.GetN()==0)
		{
			break;
		}

		int m;
		unsigned key;
		YSBOOL watch;

		for(m=0; m<todo.GetN(); m++)
		{
			plHd=todo[m];

			key=newShell.GetSearchKey(plHd);

			watch=YSFALSE;
			// if(0)
			// {
			// 	watch=YSTRUE;
			// 	newShell.SetColorOfPolygon(plHd,YsGreen());
			// }

			nextPlHd=newShell.FindNextPolygon(plHd);
			plg=newShell.GetPolygon(plHd);

			if(YsBlendShell2GetNormalOfPolygon(nom,newShell,plHd)!=YSOK)
			{
				YsErrOut("\n");
				YsErrOut("Cannot compute a normal for a polygon.  Skipping it(1).\n");
				continue;
			}

			att.SetForwardVector(nom);
			att.GetMatrix4x4(prjMat);

		RETRY:
			toBeDeleted.Set(0,NULL);
			if(YsBlendShell2FindOverlap(samePlnPlHd,newShell,ltc,plHd,cosThr,singleShellOp)==YSOK)
			{
				int i;
				if(watch==YSTRUE)
				{
					for(i=0; i<samePlnPlHd.GetN(); i++)
					{
						YsPrintf("{%d}",newShell.GetSearchKey(samePlnPlHd[i]));
					}
					YsPrintf("\n");
				}

				for(i=0; i<samePlnPlHd.GetN(); i++)
				{
					if(YsBlendShell2CheckAndKillCLoop
				    	(createdPlHd,newShell,ltc,plHd,samePlnPlHd[i],prjMat,singleShellOp)==YSTRUE)
				   	{
						// Because C-Loop might be slashed by a new polygons.

						// 2004/12/10 plHd may be deleted in the function.
						//            If plHd is included in todo[m+1] to todo[todo.GetN()-1],
						//            it should be deleted.
						int k;                            // 2004/12/10
						for(k=m+1; k<todo.GetN(); k++)    // 2004/12/10
						{                                 // 2004/12/10
							if(todo[k]==samePlnPlHd[i])   // 2004/12/10
							{                             // 2004/12/10
								todo.Delete(k);           // 2004/12/10
							}                             // 2004/12/10
						}                                 // 2004/12/10

						goto RETRY;
					}
				}

				YsBlendShell2BlendOverlap(toBeDeleted,newShell,ltc,plHd,samePlnPlHd,prjMat);
			}

			int i;
			for(i=0; i<toBeDeleted.GetN(); i++)
			{
				if(nextPlHd==toBeDeleted[i])
				{
					nextPlHd=newShell.FindNextPolygon(nextPlHd);
				}
				newShell.DeletePolygon(toBeDeleted[i]);
			}
		}
	}

	return YSOK;
}

static YSRESULT YsBlendShell2GetNormalOfPolygon(YsVec3 &nom,YsShell &shl,YsShellPolygonHandle plHd)
{
	shl.GetNormalOfPolygon(nom,plHd);
	if(nom==YsOrigin())
	{
		YsArray <YsVec3> plVtPos;
		shl.GetVertexListOfPolygon(plVtPos,plHd);
		return YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos);
	}
	return YSOK;
}

static YSRESULT YsBlendShell2GetVertexListOfPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec3,16> &plVtPos,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &/*prjMat*/)
{
	int i;
	plVtHd.Set(shl.GetNumVertexOfPolygon(plHd),shl.GetVertexListOfPolygon(plHd));
	plVtPos.Set(plVtHd.GetN(),NULL);
	for(i=0; i<plVtHd.GetN(); i++)
	{
		shl.GetVertexPosition(plVtPos[i],plVtHd[i]);
	}
	return YSOK;
}

static YSRESULT YsBlendShell2GetProjectedPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec3,16> &plVtPos,
	 YsArray <YsVec2,16> &plVtPos2,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &prjMat)
{
	int i;
	YsVec3 p;
	YsBlendShell2GetVertexListOfPolygon(plVtHd,plVtPos,shl,plHd,prjMat);
	plVtPos2.Set(plVtPos.GetN(),NULL);
	for(i=0; i<plVtPos.GetN(); i++)
	{
		prjMat.MulInverse(p,plVtPos[i],0.0);
		plVtPos2[i].Set(p.x(),p.y());
	}
	return YSOK;
}

static YSRESULT YsBlendShell2GetProjectedPolygon
	(YsArray <YsShellVertexHandle,16> &plVtHd,
	 YsArray <YsVec2,16> &plVtPos2,
	 YsShell &shl,YsShellPolygonHandle plHd,const YsMatrix4x4 &prjMat)
{
	 YsArray <YsVec3,16> plVtPos;
	 return YsBlendShell2GetProjectedPolygon(plVtHd,plVtPos,plVtPos2,shl,plHd,prjMat);
}


static YSBOOL YsBlendShell2CheckAndKillCLoop
    (YsArray <YsShellPolygonHandle,16> &createdPlHd,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsShellPolygonHandle pLoopPlHd,
     const YsMatrix4x4 &prjMat,YSBOOL singleShellOp)
{
	YsArray <YsShellVertexHandle,16> plVtHd;
	YsArray <YsVec2,16> plVtPos;
	YsShellPolygon *plg;
	YsArray <YsShellVertexHandle,16> testPlVtHd;
	YsArray <YsVec2,16> testPlVtPos;
	YsArray <YsShellVertexHandle,16> insideVtHd;
	YsArray <YsVec2,16> insideVtPos;

	createdPlHd.Set(0,NULL);

	YsBlendShell2GetProjectedPolygon(plVtHd,plVtPos,newShell,plHd,prjMat);
	plg=newShell.GetPolygon(plHd);


	YsShellPolygon *testPlg;

	YsBlendShell2GetProjectedPolygon(testPlVtHd,testPlVtPos,newShell,pLoopPlHd,prjMat);
	testPlg=newShell.GetPolygon(pLoopPlHd);

	if(testPlg->freeAttribute2!=plg->freeAttribute2 || (YSTRUE==singleShellOp && pLoopPlHd!=plHd))
	{
		int j;
		int nBoundary,nInside;
		nBoundary=0;
		nInside=0;
		insideVtHd.Set(0,NULL);
		insideVtPos.Set(0,NULL);

		// 2013/11/15 >>
		YsArray <YsShellVertexHandle,4> boundaryVtHdArray;
		// 2013/11/15 <<

		for(j=0; j<plVtPos.GetN(); j++)
		{
			YSSIDE side;
			side=YsCheckInsidePolygon2(plVtPos[j],testPlVtPos.GetN(),testPlVtPos);
			if(side==YSINSIDE)
			{
				nInside++;
				insideVtHd.Append(plVtHd[j]);
				insideVtPos.Append(plVtPos[j]);
			}
			else if(side==YSBOUNDARY)
			{
				nBoundary++;
				// 2013/11/15 >>
				boundaryVtHdArray.Append(plVtHd[j]);
				// 2013/11/15 <<
			}
			else if(side==YSOUTSIDE)
			{
				return YSFALSE;
			}
		}

		if(nBoundary<=1 && nInside>1)  // Implies C-Loop
		{
			YsArray <YSHASHKEY> newPlKeyList;

			if(YsShellBlend2InsertVertexToPolygon
			     (createdPlHd,newShell,pLoopPlHd,
			      newPlKeyList,insideVtHd[0],ltc,testPlVtPos,insideVtPos[0])==YSOK)
			{
				YsPrintf("Cloop* %d %d\n",newPlKeyList[0],newPlKeyList[1]);
				return YSTRUE;
			}
		}
		// 2013/11/15 >>
		else if(2<=nBoundary && 0<nInside) // Need 0<nInside.  Triangle can stick outside of a concave polygon.
		{
			YSBOOL bailOut=YSFALSE;
			// All boundary vertices must be used by the p-loop polygon.
			for(YSSIZE_T idx=0; idx<boundaryVtHdArray.GetN(); ++idx)
			{
				if(YSTRUE!=testPlVtHd.IsIncluded(boundaryVtHdArray[idx]))
				{
					bailOut=YSTRUE;
					break;
				}
			}

			// Make sure no edge-shareing and also the plHd is inside pLoopPlHd.
			for(YSSIZE_T cIdx=0; cIdx<plVtPos.GetN() && YSTRUE!=bailOut; ++cIdx)
			{
				const YsVec2 cen=(plVtPos[cIdx]+plVtPos.GetCyclic(cIdx+1))/2.0;
				if(YSINSIDE!=YsCheckInsidePolygon2(cen,testPlVtPos.GetN(),testPlVtPos))
				{
					bailOut=YSTRUE;
					break;
				}

				for(YSSIZE_T pIdx=0; pIdx<testPlVtPos.GetN() && YSTRUE!=bailOut; ++pIdx)
				{
					if(YSTRUE==YsSameEdge(plVtHd[cIdx],plVtHd.GetCyclic(cIdx+1),testPlVtHd[pIdx],testPlVtHd.GetCyclic(pIdx+1)))
					{
						bailOut=YSTRUE;
						break;
					}
				}
			}

			YsArray <YSHASHKEY> newPlKeyList;
			if(YSTRUE!=bailOut &&
			   YsShellBlend2InsertVertexToPolygon
			     (createdPlHd,newShell,pLoopPlHd,
			      newPlKeyList,insideVtHd[0],ltc,testPlVtPos,insideVtPos[0])==YSOK)
			{
				YsPrintf("Cloop* %d %d (Pattern 2)\n",newPlKeyList[0],newPlKeyList[1]);
				return YSTRUE;
			}
		}
		// 2013/11/15 <<
	}

	return YSFALSE;
}

static YSRESULT YsBlendShell2BlendOverlap
    (YsArray <YsShellPolygonHandle,16> &toBeDeleted,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &samePlnPlHd,
     const YsMatrix4x4 &prjMat)
{
	int i;

	for(i=0; i<samePlnPlHd.GetN(); i++)
	{
		YsBlendShell2StitchNodeOfOverlapPolygon(newShell,plHd,samePlnPlHd[i],prjMat,ltc);
		YsBlendShell2StitchEdgeOfOverlapPolygon(newShell,plHd,samePlnPlHd[i],prjMat,ltc);
	}

	// Now sameplanepolygons are stitched up together.
	// Next: Based on the stitched info, subdivide polygons.

	YSRESULT res;
	res=YsBlendShell2BlendOverlapAfterStitch(toBeDeleted,newShell,ltc,plHd,samePlnPlHd,prjMat);

	return res;
}

static YSRESULT YsBlendShell2BlendOverlapAfterStitch
    (YsArray <YsShellPolygonHandle,16> &toBeDeleted,
     YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &samePlnPlHd,
     const YsMatrix4x4 &prjMat)
{
	YsArray <YsArray <YsShellVertexHandle,16> > newPlVtHdList;
	YsArray <YsArray <YsVec2,16> > newPlVtPosList;

	YsArray <YsShellVertexHandle,16> plVtHd,samePlnPlVtHd;
	YsArray <YsVec2,16> plVtPos,samePlnPlVtPos;

	YSSIZE_T i;
	YsShellPolygon *plg,*samePlnPlg;
	YsVec3 nom;
	YsColor col;
	YSBOOL watch;

	plg=newShell.GetPolygon(plHd);
	YsBlendShell2GetProjectedPolygon(plVtHd,plVtPos,newShell,plHd,prjMat);
	newShell.GetNormalOfPolygon(nom,plHd);
	newShell.GetColorOfPolygon(col,plHd);

	for(i=samePlnPlHd.GetN()-1; i>=0; i--)
	{
		watch=YSFALSE;

		samePlnPlg=newShell.GetPolygon(samePlnPlHd[i]);
		if(samePlnPlg->freeAttribute2!=plg->freeAttribute2)
		{
			YsBlendShell2GetProjectedPolygon(samePlnPlVtHd,samePlnPlVtPos,newShell,samePlnPlHd[i],prjMat);

			if(watch==YSTRUE)
			{
				int j;
				YsPrintf("(B)\n");
				for(j=0; j<samePlnPlVtHd.GetN(); j++)
				{
					YsPrintf("(%d)",newShell.GetSearchKey(samePlnPlVtHd[j]));
				}
				YsPrintf("\n");
			}

			newPlVtHdList.Set(0,NULL);
			newPlVtPosList.Set(0,NULL);
			YsShellBlend2SubdivOverlappingPolygon
			    (newPlVtHdList,newPlVtPosList,newShell,plVtHd,plVtPos,samePlnPlVtHd,samePlnPlVtPos,watch);

			if(watch==YSTRUE)
			{
				YsPrintf("%d\n",(int)newPlVtHdList.GetN());
			}
			if(newPlVtHdList.GetN()>1)
			{
				toBeDeleted.Append(plHd);
				ltc.DeletePolygon(plHd);

				int k;
				for(k=0; k<newPlVtHdList.GetN(); k++)
				{
					YsShellPolygonHandle newPlHd;
					YsShellPolygon *newPlg;

					newPlHd=newShell.AddPolygonH(newPlVtHdList[k].GetN(),newPlVtHdList[k]);

					newPlg=newShell.GetPolygon(newPlHd);
					newPlg->freeAttribute0=plg->freeAttribute0; // 2002/12/24
					newPlg->freeAttribute1=plg->freeAttribute1; // 2002/12/24
					newPlg->freeAttribute2=plg->freeAttribute2;
					newPlg->freeAttribute3=plg->freeAttribute3;
					newPlg->freeAttribute4=plg->freeAttribute4;
					newShell.SetColorOfPolygon(newPlHd,col);
					newShell.SetNormalOfPolygon(newPlHd,nom);

					ltc.AddPolygon(newPlHd);
					YsBlendShell2BlendOverlapAfterStitch
					    (toBeDeleted,newShell,ltc,newPlHd,samePlnPlHd,prjMat);
				}
				return YSOK;
			}
		}
	}
	return YSOK;
}

/* Botsu
{
	int i,j;
	YsArray <YsShellVertexHandle,16> masterPlVtHd;
	YsArray <YsVec2,16> masterPlVtPos;

	// Save geometry and vertex list of the master polygon: it will be the cutter later
	masterPlVtHd.Set(newShell.GetNumVertexOfPolygon(plHd),newShell.GetVertexListOfPolygon(plHd));
	for(i=0; i<masterPlVtHd.GetN(); i++)
	{
		YsVec3 vtPos;
		YsVec2 vtPos2;
		newShell.GetVertexPosition(vtPos,masterPlVtHd[i]);
		prjMat.MulInverse(vtPos,vtPos,0.0);
		vtPos2.Set(vtPos.x(),vtPos.y());
		masterPlVtPos.Append(vtPos2);
	}

	YsArray <YsArray <YsShellVertexHandle,16> > newPlVtHdList1;
	YsArray <YsArray <YsVec2,16> > newPlVtPosList1;
	YsArray <YsShellPolygonHandle,16> masterPlHdList;


	// 1st loop subdivide masterPlHd (=plHd) by samePlnPlHd's
	masterPlHdList.Set(0,NULL);
	masterPlHdList.Append(plHd);
	for(i=0; i<samePlnPlHd.GetN(); i++)
	{
		YsArray <YsShellVertexHandle,16> plVtHd[2];
		YsArray <YsVec2,16> plVtPos[2];

		plVtHd[1].Set
		    (newShell.GetNumVertexOfPolygon(samePlnPlHd[i]),
		     newShell.GetVertexListOfPolygon(samePlnPlHd[i]));

		for(j=0; j<masterPlHdList.GetN(); j++)
		{
			plVtHd[0].Set
			    (newShell.GetNumVertexOfPolygon(masterPlHdList[j]),
			     newShell.GetVertexListOfPolygon(masterPlHdList[j]));

			int k;
			for(k=0; k<2; k++)
			{
				YsVec3 vtPos;
				YsVec2 vtPos2;
				int m;
				for(m=0; m<plVtHd[k].GetN(); m++)
				{
					newShell.GetVertexPosition(vtPos,plVtHd[k][m]);
					prjMat.MulInverse(vtPos,vtPos,0.0);
					vtPos2.Set(vtPos.x(),vtPos.y());
					plVtPos[k].Append(vtPos2);
				}
			}

			newPlVtHdList1.Set(0,NULL);
			newPlVtPosList1.Set(0,NULL);
			YsShellBlend2SubdivOverlappingPolygon
			    (newPlVtHdList1,newPlVtPosList1,newShell,plVtHd[0],plVtPos[0],plVtHd[1],plVtPos[1]);


			YsShellPolygonHandle newPlHd;
			YsShellPolygon *masterPlg,*samePlnPlg,*newPlg;
			YsVec3 nom;
			YsColor col;

			masterPlg=newShell.GetPolygon(masterPlHdList[j]);
			samePlnPlg=newShell.GetPolygon(samePlnPlHd[i]);

			if(newPlVtHdList1.GetN()>1)  // i.e., # of polygon increased meaning masterPlHd[j] is subdivided
			{
				toBeDeleted.Append(masterPlHdList[j]);
				newShell.GetNormalOfPolygon(nom,masterPlHdList[j]);
				newShell.GetColorOfPolygon(col,masterPlHdList[j]);

				int k;
				for(k=0; k<newPlVtHdList1.GetN(); k++)
				{
					newPlHd=newShell.AddPolygonH(newPlVtHdList1[k].GetN(),newPlVtHdList1[k]);
					newPlg=newShell.GetPolygon(newPlHd);
					newPlg->freeAttribute2=masterPlg->freeAttribute2;
					newPlg->freeAttribute3=masterPlg->freeAttribute3;
					newPlg->freeAttribute4=masterPlg->freeAttribute4;
					newShell.SetColorOfPolygon(newPlHd,col);
					newShell.SetNormalOfPolygon(newPlHd,nom);

					YsVec2 aip;
					if(YsGetArbitraryInsidePointOfPolygon2
					    (aip,newPlVtPosList1[k].GetN(),newPlVtPosList1[k])==YSOK &&
					   YsCheckInsidePolygon2(aip,plVtPos[1].GetN(),plVtPos[1])==YSINSIDE)
					{
						if(masterPlg->freeAttribute2==0 && samePlnPlg->freeAttribute2==1)
						{
							newPlg->freeAttribute2=2;
							newPlg->freeAttribute3=masterPlg->freeAttribute3;
							newPlg->freeAttribute4=samePlnPlg->freeAttribute3;
						}
						else if(masterPlg->freeAttribute2==1 && samePlnPlg->freeAttribute2==0)
						{
							newPlg->freeAttribute2=2;
							newPlg->freeAttribute3=samePlnPlg->freeAttribute3;
							newPlg->freeAttribute4=masterPlg->freeAttribute3;
						}
						else if(masterPlg->freeAttribute2!=2)
						{
							printf("!(1) %d %d\n",masterPlg->freeAttribute2,samePlnPlg->freeAttribute2);
						}
					}
					ltc.AddPolygon(newPlHd);
					masterPlHdList.Append(newPlHd);
				}

				ltc.DeletePolygon(masterPlHdList[j]);
				masterPlHdList.Delete(j);
			}
			else // masterPlHdList[j] is not subdivided.
			     // But, it is still possible that masterPlHdList[j] is already overlapping
			{
				YsVec2 aip;

				if(YsGetArbitraryInsidePointOfPolygon2(aip,plVtPos[0].GetN(),plVtPos[0])==YSOK &&
				   YsCheckInsidePolygon2(aip,plVtPos[1].GetN(),plVtPos[1])==YSINSIDE)
				{
					if(masterPlg->freeAttribute2==0 && samePlnPlg->freeAttribute2==1)
					{
						newPlg->freeAttribute2=2;
						newPlg->freeAttribute3=masterPlg->freeAttribute3;
						newPlg->freeAttribute4=samePlnPlg->freeAttribute3;
					}
					else if(masterPlg->freeAttribute2==1 && samePlnPlg->freeAttribute2==0)
					{
						newPlg->freeAttribute2=3;
						newPlg->freeAttribute3=samePlnPlg->freeAttribute3;
						newPlg->freeAttribute4=masterPlg->freeAttribute3;
					}
					else if(masterPlg->freeAttribute2!=2 && masterPlg->freeAttribute2!=3)
					{
						printf("!(2) %d %d\n",masterPlg->freeAttribute2,samePlnPlg->freeAttribute2);
					}
				}
			}
		}
	}


	// 2nd loop: subdivide samePlnPlHd's
	for(i=0; i<samePlnPlHd.GetN(); i++)
	{
		YsArray <YsShellVertexHandle,16> plVtHd;
		YsArray <YsVec2,16> plVtPos;

		plVtHd.Set
		    (newShell.GetNumVertexOfPolygon(samePlnPlHd[i]),
		     newShell.GetVertexListOfPolygon(samePlnPlHd[i]));

		YsVec3 vtPos;
		YsVec2 vtPos2;
		int m;
		for(m=0; m<plVtHd.GetN(); m++)
		{
			newShell.GetVertexPosition(vtPos,plVtHd[m]);
			prjMat.MulInverse(vtPos,vtPos,0.0);
			vtPos2.Set(vtPos.x(),vtPos.y());
			plVtPos.Append(vtPos2);
		}

		newPlVtHdList1.Set(0,NULL);
		newPlVtPosList1.Set(0,NULL);
		YsShellBlend2SubdivOverlappingPolygon
		    (newPlVtHdList1,newPlVtPosList1,newShell,plVtHd,plVtPos,masterPlVtHd,masterPlVtPos);


		if(newPlVtHdList1.GetN()>1)
		{
			YsShellPolygonHandle newPlHd;
			YsShellPolygon *samePlnPlg,*newPlg;
			YsVec3 nom;
			YsColor col;

			toBeDeleted.Append(samePlnPlHd[i]);

			samePlnPlg=newShell.GetPolygon(samePlnPlHd[i]);
			newShell.GetNormalOfPolygon(nom,samePlnPlHd[i]);
			newShell.GetColorOfPolygon(col,samePlnPlHd[i]);

			int k;
			for(k=0; k<newPlVtHdList1.GetN(); k++)
			{
				newPlHd=newShell.AddPolygonH(newPlVtHdList1[k].GetN(),newPlVtHdList1[k]);
				newPlg=newShell.GetPolygon(newPlHd);
				ltc.AddPolygon(newPlHd);
				newShell.SetColorOfPolygon(newPlHd,col);
				newShell.SetNormalOfPolygon(newPlHd,nom);

				YsVec2 aip;
				if(YsGetArbitraryInsidePointOfPolygon2
				    (aip,newPlVtPosList1[k].GetN(),newPlVtPosList1[k])==YSOK &&
				   YsCheckInsidePolygon2(aip,masterPlVtPos.GetN(),masterPlVtPos)!=YSINSIDE)
				{
					newPlg->freeAttribute2=samePlnPlg->freeAttribute2;
					newPlg->freeAttribute3=samePlnPlg->freeAttribute3;
					newPlg->freeAttribute4=samePlnPlg->freeAttribute4;
				}
				else
				{
					newPlg->freeAttribute2=3;
					newPlg->freeAttribute3=0;
					newPlg->freeAttribute4=0;
				}
			}

			ltc.DeletePolygon(samePlnPlHd[i]);
		}
		else // samePlnPlg[i] is not subdivided
		     // But samePlnPlg[i] may already be overlapping with masterPlg
		{
			YsVec2 aip;
			if(YsGetArbitraryInsidePointOfPolygon2(aip,plVtPos.GetN(),plVtPos)==YSOK &&
			   YsCheckInsidePolygon2(aip,masterPlVtPos.GetN(),masterPlVtPos)==YSINSIDE)
			{
				YsShellPolygon *samePlnPlg;
				samePlnPlg=newShell.GetPolygon(samePlnPlHd[i]);
				samePlnPlg->freeAttribute2=3;
				samePlnPlg->freeAttribute3=0;
				samePlnPlg->freeAttribute4=0;
			}
		}
	}

	return YSOK;
}*/

static YSRESULT YsBlendShell2RemoveOverlapBasedOnTopology(YsShell &newShell,YsShellLattice &ltc,YSBOOL singleShellOp)
{
	YsShellPolygonHandle plHd;
	const YsShellSearchTable *search;
	YsArray <YsShellPolygonHandle,16> toBeDeleted;
	YsShellPolygon *plg;
	YsArray <YsShellPolygonHandle,16> plHdList; // const YsArray <YsShellPolygonHandle> *plHdList;
	YSBOOL watch;

	search=newShell.GetSearchTable();


	watch=YSFALSE;


	plHd=NULL;
	while((plHd=newShell.FindNextPolygon(plHd))!=NULL)
	{
		// watch=YSTRUE;


		plg=newShell.GetPolygon(plHd);
		if(plg->freeAttribute2==0 || YSTRUE==singleShellOp)
		{
			int nPlVt;
			const YsShellVertexHandle *plVtHd;

			nPlVt=newShell.GetNumVertexOfPolygon(plHd);
			plVtHd=newShell.GetVertexListOfPolygon(plHd);
			search->FindPolygonListByVertexList(plHdList,newShell,nPlVt,plVtHd);

			if(watch==YSTRUE)
			{
				YsPrintf("Watch! %d\n",(int)plHdList.GetN());
			}

			if(plHdList.GetN()>0)
			{
				if(watch==YSTRUE)
				{
					YsPrintf("Watch! %d\n",(int)plHdList.GetN());
				}

				int i;
				for(i=0; i<plHdList.GetN(); i++)
				{
					if(watch==YSTRUE)
					{
						int j;
						YsPrintf("(1)");
						for(j=0; j<nPlVt; j++)
						{
							YsPrintf("[%d]",newShell.GetSearchKey(plVtHd[j]));
						}
						YsPrintf("\n");
						int nVsPlVt;
						const YsShellVertexHandle *vsPlVtHd;
						nVsPlVt=newShell.GetNumVertexOfPolygon(plHdList[i]);
						vsPlVtHd=newShell.GetVertexListOfPolygon(plHdList[i]);
						YsPrintf("	(2)");
						for(j=0; j<nVsPlVt; j++)
						{
							YsPrintf("[%d]",newShell.GetSearchKey(vsPlVtHd[j]));
						}
						YsPrintf("\n");
					}

					YsShellPolygon *overlapPlg;
					overlapPlg=newShell.GetPolygon(plHdList[i]);
					if((overlapPlg->freeAttribute2!=plg->freeAttribute2 || (YSTRUE==singleShellOp && plHdList[i]!=plHd)) &&
					   newShell.GetNumVertexOfPolygon(plHdList[i])==nPlVt)
					{
						plg->freeAttribute2=2;
						plg->freeAttribute3=plg->freeAttribute3;
						plg->freeAttribute4=overlapPlg->freeAttribute3;
						toBeDeleted.Append(plHdList[i]);
						if(watch==YSTRUE)
						{
							YsPrintf("Watch! Overlap found.\n");
						}
						goto NEXTPOLYGON;
					}
				}
			}
		}
	NEXTPOLYGON:
		;
	}

	int i;
	for(i=0; i<toBeDeleted.GetN(); i++)
	{
		ltc.DeletePolygon(toBeDeleted[i]);
		newShell.DeletePolygon(toBeDeleted[i]);
	}

	return YSOK;
}

static YSRESULT YsBlendShell2FindOverlap
    (YsArray <YsShellPolygonHandle,16> &samePlnPlHd,YsShell &newShell,YsShellLattice &ltc,
     YsShellPolygonHandle plHd,const double &cosThr,YSBOOL singleShellOp)
{
	YsShellPolygon *plg;
	YsArray <YsVec3> plVtPos;
	YsVec3 nom;
	YsArray <YsShellPolygonHandle,16> collidePlHd;
	YsCollisionOfPolygon wizard;

	samePlnPlHd.Set(0,NULL);

	plg=newShell.GetPolygon(plHd);

	newShell.GetVertexListOfPolygon(plVtPos,plHd);
	newShell.GetNormalOfPolygon(nom,plHd);
	if(nom==YsOrigin())
	{
		if(YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos)!=YSOK)
		{
			YsErrOut("\n");
			YsErrOut("Cannot compute a normal for a polygon.  Skipping it(1).\n");
			return YSERR;
		}
	}

	wizard.SetPolygon1(plVtPos.GetN(),plVtPos);

	if(ltc.MakePossibllyCollidingPolygonList(collidePlHd,plVtPos.GetN(),plVtPos)==YSOK && collidePlHd.GetN()>1)
	{
		int i;
		for(i=0; i<collidePlHd.GetN(); i++)
		{
			YsShellPolygon *againstPlg;

			againstPlg=newShell.GetPolygon(collidePlHd[i]);
			if(againstPlg->freeAttribute2!=plg->freeAttribute2 || (YSTRUE==singleShellOp && collidePlHd[i]!=plHd))
			{
				YsArray <YsVec3> againstPlVtPos;
				YsVec3 againstNom;
				newShell.GetVertexListOfPolygon(againstPlVtPos,collidePlHd[i]);
				newShell.GetNormalOfPolygon(againstNom,collidePlHd[i]);
				if(againstNom==YsOrigin())
				{
					if(YsGetAverageNormalVector(againstNom,againstPlVtPos.GetN(),againstPlVtPos)!=YSOK)
					{
						YsErrOut("\n");
						YsErrOut("Cannot compute a normal for a polygon.  Skipping it(2).\n");
						goto NEXTPOLYGON;
					}
				}

				if(YsAbs(nom*againstNom)>=cosThr)
				{
					if(YsAbs((againstPlVtPos[0]-plVtPos[0])*nom)<YsTolerance)
					{
						samePlnPlHd.Append(collidePlHd[i]);
					}
				}
			}

		NEXTPOLYGON:
			;
		}
	}

	return YSOK;
}

static YSRESULT YsBlendShell2StitchNodeOfOverlapPolygon
    (YsShell &newShell,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsMatrix4x4 &prjMat,
     YsShellLattice &ltc)
{
	YsVec3 edVtPos[2],testVtPos;
	YsVec2 edVtPos2[2],testVtPos2,nearPos2;

	YsArray <YsShellVertexHandle> plVtHd[2];
	newShell.GetVertexListOfPolygon(plVtHd[0],plHd1);
	newShell.GetVertexListOfPolygon(plVtHd[1],plHd2);

	int i,j,k,fr,to;
	for(i=0; i<2; i++)
	{
		fr=i;
		to=(i+1)%2;
		for(j=0; j<plVtHd[fr].GetN(); j++)
		{
			for(k=0; k<plVtHd[to].GetN(); k++)
			{
				if(plVtHd[fr][j]!=plVtHd[to][k] && plVtHd[fr][j]!=plVtHd[to][(k+1)%plVtHd[to].GetN()])
				{
					newShell.GetVertexPosition(testVtPos,plVtHd[fr][j]);
					newShell.GetVertexPosition(edVtPos[0],plVtHd[to][k]);
					newShell.GetVertexPosition(edVtPos[1],plVtHd[to][(k+1)%plVtHd[to].GetN()]);

					prjMat.MulInverse(testVtPos,testVtPos,0.0);
					prjMat.MulInverse(edVtPos[0],edVtPos[0],0.0);
					prjMat.MulInverse(edVtPos[1],edVtPos[1],0.0);

					testVtPos2.Set(testVtPos.x(),testVtPos.y());
					edVtPos2[0].Set(edVtPos[0].x(),edVtPos[0].y());
					edVtPos2[1].Set(edVtPos[1].x(),edVtPos[1].y());

					if(testVtPos2==edVtPos2[0])
					{
						YsShellBlend2SubstVtHd(newShell,plVtHd[to][k],plVtHd[fr][j]);
						plVtHd[to][k]=plVtHd[fr][j];
					}
					else if(testVtPos2==edVtPos2[1])
					{
						YsShellBlend2SubstVtHd(newShell,plVtHd[to][(k+1)%plVtHd[to].GetN()],plVtHd[fr][j]);
						plVtHd[to][(k+1)%plVtHd[to].GetN()]=plVtHd[fr][j];
					}
					else if(YsGetNearestPointOnLine2(nearPos2,edVtPos2[0],edVtPos2[1],testVtPos2)==YSOK &&
					        testVtPos2==nearPos2 &&
					        YsCheckInBetween2(nearPos2,edVtPos2[0],edVtPos2[1])==YSTRUE)
					{
						YsShellBlend2InsertVertexToEdge
						    (newShell,plVtHd[to][k],plVtHd[to][(k+1)%plVtHd[to].GetN()],plVtHd[fr][j],ltc);
						plVtHd[to].Insert(k+1,plVtHd[fr][j]);
					}
				}
			}
		}
	}

	return YSOK;
}

static YSRESULT YsBlendShell2StitchEdgeOfOverlapPolygon
    (YsShell &newShell,YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsMatrix4x4 &prjMat,
     YsShellLattice &ltc)
{
	YsVec3 edVtPos3[2][2],vtPos;
	YsVec2 edVtPos2[2][2],is;
	YsShellVertexHandle edVtHd[2][2];
	double depth;
	YsArray <YsShellVertexHandle> plVtHd[2];

	newShell.GetVertexListOfPolygon(plVtHd[0],plHd1);
	newShell.GetVertexListOfPolygon(plVtHd[1],plHd2);

	int i,j;
	for(i=0; i<plVtHd[0].GetN(); i++)
	{
		edVtHd[0][0]=plVtHd[0][i];
		edVtHd[0][1]=plVtHd[0][(i+1)%plVtHd[0].GetN()];

		newShell.GetVertexPosition(edVtPos3[0][0],edVtHd[0][0]);
		prjMat.MulInverse(vtPos,edVtPos3[0][0],0.0);
		edVtPos2[0][0].Set(vtPos.x(),vtPos.y());

		newShell.GetVertexPosition(edVtPos3[0][1],edVtHd[0][1]);
		prjMat.MulInverse(vtPos,edVtPos3[0][1],0.0);
		edVtPos2[0][1].Set(vtPos.x(),vtPos.y());

		for(j=0; j<plVtHd[1].GetN(); j++)
		{
			edVtHd[1][0]=plVtHd[1][j];
			edVtHd[1][1]=plVtHd[1][(j+1)%plVtHd[1].GetN()];

			if(edVtHd[0][0]!=edVtHd[1][0] && edVtHd[0][0]!=edVtHd[1][1] &&
			   edVtHd[0][1]!=edVtHd[1][0] && edVtHd[0][1]!=edVtHd[1][1])
			{
				newShell.GetVertexPosition(edVtPos3[1][0],edVtHd[1][0]);
				prjMat.MulInverse(vtPos,edVtPos3[1][0],0.0);
				edVtPos2[1][0].Set(vtPos.x(),vtPos.y());

				newShell.GetVertexPosition(edVtPos3[1][1],edVtHd[1][1]);
				prjMat.MulInverse(vtPos,edVtPos3[1][1],0.0);
				edVtPos2[1][1].Set(vtPos.x(),vtPos.y());

				depth=vtPos.z();

				if(YsGetLineIntersection2(is,edVtPos2[0][0],edVtPos2[0][1],edVtPos2[1][0],edVtPos2[1][1])==YSOK &&
				   YsCheckInBetween2(is,edVtPos2[0][0],edVtPos2[0][1])==YSTRUE &&
				   YsCheckInBetween2(is,edVtPos2[1][0],edVtPos2[1][1])==YSTRUE)
				{
					if(is==edVtPos2[0][0] || is==edVtPos2[0][1] || is==edVtPos2[1][0] || is==edVtPos2[1][1])
					{
						YsErrOut("Warning : Node is not stitched(?)\n");
					}
					else
					{
						YsVec3 nea1,nea2,newVtPos;
						if(YsGetNearestPointOfTwoLine
						     (nea1,nea2,edVtPos3[0][0],edVtPos3[0][1],edVtPos3[1][0],edVtPos3[1][1])==YSOK)
						{
							newVtPos=(nea1+nea2)/2.0;
						}
						else
						{
							newVtPos.Set(is.x(),is.y(),vtPos.z());
							prjMat.Mul(newVtPos,newVtPos,0.0);
						}

						YsShellVertexHandle newVtHd;
						newVtHd=newShell.AddVertexH(newVtPos);
						newShell.GetVertex(newVtHd)->freeAttribute3=-1;
						newShell.GetVertex(newVtHd)->freeAttribute4=-1;

						YsShellBlend2InsertVertexToEdge(newShell,edVtHd[0][0],edVtHd[0][1],newVtHd,ltc);
						YsShellBlend2InsertVertexToEdge(newShell,edVtHd[1][0],edVtHd[1][1],newVtHd,ltc);
						plVtHd[0].Insert(i+1,newVtHd);
						plVtHd[1].Insert(j+1,newVtHd);

						edVtHd[0][1]=newVtHd;
						edVtPos3[0][1]=newVtPos;
						prjMat.MulInverse(vtPos,newVtPos,0.0);
						edVtPos2[0][1].Set(vtPos.x(),vtPos.y());
					}
				}
			}
		}
	}

	return YSOK;
}

static YSRESULT YsShellBlend2SubdivOverlappingPolygon
    (YsArray <YsArray <YsShellVertexHandle,16> > &newPlVtHdList,
     YsArray <YsArray <YsVec2,16> > &newPlVtPosList,
     YsShell &newShell,
     const YsArray <YsShellVertexHandle,16> &plVtHd,
     const YsArray <YsVec2,16> &plVtPos,
     const YsArray <YsShellVertexHandle,16> &cutterVtHd,
     const YsArray <YsVec2,16> &cutterVtPos,
     YSBOOL watch)
{
	// Idea: Subdivide plVtHd(&plVtPos synchronously).  If plVtHd is not subdividable, just add
    //       plVtHd to newPlVtHdList and return.  Otherwise, launch next recursion.

	int i,j,firstI,firstJ,endI,endJ,step;
	YsArray <YsShellVertexHandle> cutterPath;
	YsArray <YsVec2> cutterPathPos;
	YsHashTable <int> plVtIdToCutterVtId(61),cutterVtIdToPlVtId(61);

	// (1) Find shared vertices
	int nShared;
	nShared=0;
	for(i=0; i<plVtHd.GetN(); i++)
	{
		for(j=0; j<cutterVtHd.GetN(); j++)
		{
			if(plVtHd[i]==cutterVtHd[j])
			{
				plVtIdToCutterVtId.AddElement(unsigned(i),j);
				cutterVtIdToPlVtId.AddElement(unsigned(j),i);
				nShared++;
			}
		}
	}
	if(nShared==0)
	{
		goto NOSUBDIVISION;
	}


	// (2) Attempt cutting
	for(i=0; i<plVtHd.GetN(); i++)
	{
		firstI=i;
		if(plVtIdToCutterVtId.FindElement(firstJ,unsigned(i))==YSOK)
		{
			for(step=-1; step<=1; step+=2)
			{
				int n;
				YsVec2 midEdge;

				cutterPath.Set(0,NULL);
				cutterPathPos.Set(0,NULL);

				cutterPath.Append(cutterVtHd[firstJ]);
				cutterPathPos.Append(cutterVtPos[firstJ]);
				for(n=1; n<cutterVtHd.GetN(); n++)
				{
					YSSIZE_T idx;
					idx=(firstJ+cutterVtHd.GetN()+n*step)%cutterVtHd.GetN();

					midEdge=(cutterPathPos.GetEndItem()+cutterVtPos[idx])/2.0;

					cutterPath.Append(cutterVtHd[idx]);
					cutterPathPos.Append(cutterVtPos[idx]);


					if(YsCheckInsidePolygon2(midEdge,plVtPos.GetN(),plVtPos)!=YSINSIDE)
					{
						if(watch==YSTRUE)
						{
							YsPrintf("Outside mid-edge-point (%lf %lf)\n",midEdge.x(),midEdge.y());
							YsPrintf("# of cutter path=%d\n",(int)cutterPath.GetN());
						}
						break;
					}
					else if(cutterVtIdToPlVtId.FindElement(endI,unsigned(idx))==YSOK)
					{
						// Found cutter path?
						if(watch==YSTRUE)
						{
							YsPrintf("Found cutter path\n");
						}

						YSSIZE_T m;

						endJ=(int)idx;

						if(endI<firstI)
						{
							YsSwapSomething <int> (endI,firstI);
							for(m=0; m<cutterPath.GetN()/2; m++)
							{
								YsSwapSomething <YsShellVertexHandle>
								    (cutterPath[m],cutterPath[cutterPath.GetN()-1-m]);
							}
						}

						// This check may destroy the process.
						// This check is also redundant & unnecesssarily overrestrictive.
						// So, just leave it commented out.
						// if(endI-firstI<=1 && YsAbs(endJ-firstJ)<=1)
						// {
						// 	if(watch==YSTRUE)
						// 	{
						// 		YsPrintf("short edge. breaking.\n");
						// 	}
						// 	break;
						// }

						if(plVtHd[firstI]!=cutterPath[0])
						{
							YsErrOut("Inconsistency! (0)\n");
							break;
						}
						else if(plVtHd[endI]!=cutterPath.GetEndItem())
						{
							YsErrOut("Inconsistency! (1)\n");
							break;
						}

						YsArray <YsShellVertexHandle,16> plVtHd1,plVtHd2;
						YsArray <YsVec2,16> plVtPos1,plVtPos2;
						for(m=0; m<firstI; m++)
						{
							plVtHd1.Append(plVtHd[m]);
							plVtPos1.Append(plVtPos[m]);
						}
						for(m=0; m<cutterPath.GetN()-1; m++)
						{
							plVtHd1.Append(cutterPath[m]);
							plVtPos1.Append(cutterPathPos[m]);
						}
						for(m=endI; m<plVtHd.GetN(); m++)
						{
							plVtHd1.Append(plVtHd[m]);
							plVtPos1.Append(plVtPos[m]);
						}

						for(m=firstI; m<endI; m++)
						{
							plVtHd2.Append(plVtHd[m]);
							plVtPos2.Append(plVtPos[m]);
						}
						for(m=cutterPath.GetN()-1; m>0; m--)
						{
							plVtHd2.Append(cutterPath[m]);
							plVtPos2.Append(cutterPathPos[m]);
						}
						if(plVtHd1.GetN()>=3 && plVtHd2.GetN()>=3)
						{
							if(watch==YSTRUE)
							{
								YsPrintf("going recursive.\n");
							}

							YSRESULT res1,res2;
							res1=YsShellBlend2SubdivOverlappingPolygon
							    (newPlVtHdList,newPlVtPosList,newShell,plVtHd1,plVtPos1,cutterVtHd,cutterVtPos,watch);
							res2=YsShellBlend2SubdivOverlappingPolygon
							    (newPlVtHdList,newPlVtPosList,newShell,plVtHd2,plVtPos2,cutterVtHd,cutterVtPos,watch);
							return ((res1==YSOK && res2==YSOK) ? YSOK : YSERR);
						}

						break;
					}
					else if(YsCheckInsidePolygon2(cutterVtPos[idx],plVtPos.GetN(),plVtPos)!=YSINSIDE)
					{
						if(watch==YSTRUE)
						{
							YsPrintf("Outside vertex.\n");
							YsPrintf("# of cutter path=%d\n",(int)cutterPath.GetN());
						}
						break;
					}
				}
			}
		}
	}

NOSUBDIVISION:
	if(watch==YSTRUE)
	{
		YsPrintf("End of function.\n");
	}
	newPlVtHdList.Append(plVtHd);
	newPlVtPosList.Append(plVtPos);
	return YSOK;
}

// YSRESULT YsShellBlend2OutputOverlapInspection
// 	(const char fn[],YsShell &shl,YsShellPolygonHandle plHd,YsArray <YsShellPolygonHandle,16> &plHdList)
// {
// 	int i,j,vtId;
// 	FILE *fp;
// 	YsArray <YsVec3> plVtPos;
// 
// 	fp=fopen(fn,"w");
// 	if(fp!=NULL)
// 	{
// 		vtId=0;
// 
// 		fprintf(fp,"SURF\n");
// 		shl.GetVertexListOfPolygon(plVtPos,plHd);
// 		for(i=0; i<plVtPos.GetN(); i++)
// 		{
// 			fprintf(fp,"V %s\n",plVtPos[i].Txt());
// 		}
// 
// 		fprintf(fp,"F\n");
// 		if(shl.GetPolygon(plHd)->freeAttribute2==0)
// 		{
// 			fprintf(fp,"C 31\n");
// 		}
// 		else if(shl.GetPolygon(plHd)->freeAttribute2==1)
// 		{
// 			fprintf(fp,"C %d\n",31*32);
// 		}
// 		else
// 		{
// 			fprintf(fp,"C %d\n",31*32+31);
// 		}
// 		fprintf(fp,"V");
// 		for(i=0; i<plVtPos.GetN(); i++)
// 		{
// 			fprintf(fp," %d",vtId++);
// 		}
// 		fprintf(fp,"\n");
// 		fprintf(fp,"E\n");
// 
// 		for(i=0; i<plHdList.GetN(); i++)
// 		{
// 			shl.GetVertexListOfPolygon(plVtPos,plHdList[i]);
// 			for(j=0; j<plVtPos.GetN(); j++)
// 			{
// 				fprintf(fp,"V %s\n",plVtPos[j].Txt());
// 			}
// 			fprintf(fp,"F\n");
// 			if(shl.GetPolygon(plHdList[i])->freeAttribute2==0)
// 			{
// 				fprintf(fp,"C 31\n");
// 			}
// 			else if(shl.GetPolygon(plHdList[i])->freeAttribute2==1)
// 			{
// 				fprintf(fp,"C %d\n",31*32);
// 			}
// 			else
// 			{
// 				fprintf(fp,"C %d\n",31*32+31);
// 			}
// 
// 			fprintf(fp,"V");
// 			for(j=0; j<plVtPos.GetN(); j++)
// 			{
// 				fprintf(fp," %d",vtId++);
// 			}
// 			fprintf(fp,"\n");
// 			fprintf(fp,"E\n");
// 
// 		}
// 		fclose(fp);
// 		return YSOK;
// 	}
// 	return YSERR;
// }


