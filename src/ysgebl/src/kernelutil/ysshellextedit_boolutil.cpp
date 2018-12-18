/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_boolutil.cpp
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

#include <ysclass.h>
#include "thread"
#include "ysshellext_localop.h"
#include "ysshellextedit_boolutil.h"
#include "ysshellext_trackingutil.h"




// Verify-save
//#include <ysshellextio.h> // Needed only for verification
//	FILE *fp=fopen("c:/tmp/test.srf","w");
//	if(NULL!=fp)
//	{
//		YsTextFileOutputStream outStream(fp);
//
//		YsShellExtWriter writer;
//		writer.SaveSrf(outStream,mixed[shlId]);
//
//		fclose(fp);
//	}
//	else
//	{
//		printf("Cannot verify-save\n");
//		printf("%s %d>",__FUNCTION__,__LINE__);getchar();
//	}


YsShellExtEdit_BooleanUtil::VertexOrigin::VertexOrigin()
{
	Initialize();
}

void YsShellExtEdit_BooleanUtil::VertexOrigin::Initialize(void)
{
	vtHd[0]=NULL;
	vtHd[1]=NULL;
}

YsShellExtEdit_BooleanUtil::PolygonOrigin::PolygonOrigin()
{
	Initialize();
}

void YsShellExtEdit_BooleanUtil::PolygonOrigin::Initialize(void)
{
	plHd[0]=NULL;
	plHd[1]=NULL;
}

int YsShellExtEdit_BooleanUtil::PolygonOrigin::GetCode(void) const // freeAttribute2
{
	if(NULL!=plHd[0] && NULL!=plHd[1])
	{
		return 2;
	}
	else if(NULL!=plHd[0])
	{
		return 0;
	}
	else if(NULL!=plHd[1])
	{
		return 1;
	}
	return -1;
}

bool YsShellExtEdit_BooleanUtil::PolygonOrigin::operator==(const YsShellExtEdit_BooleanUtil::PolygonOrigin &from) const
{
	return (plHd[0]==from.plHd[0] && plHd[1]==from.plHd[1]);
}

bool YsShellExtEdit_BooleanUtil::PolygonOrigin::operator!=(const YsShellExtEdit_BooleanUtil::PolygonOrigin &from) const
{
	return (plHd[0]!=from.plHd[0] || plHd[1]!=from.plHd[1]);
}


YsShellExtEdit_BooleanUtil::YsShellExtEdit_BooleanUtil()
{
}

void YsShellExtEdit_BooleanUtil::CleanUp(void)
{
	vtKeyInMixToOrigin[0].CleanUp();
	vtKeyInMixToOrigin[1].CleanUp();
	plKeyInMixToOrigin[0].CleanUp();
	plKeyInMixToOrigin[1].CleanUp();
	ceKeyInMixToOrigin[0].CleanUp();
	ceKeyInMixToOrigin[1].CleanUp();

	vtKeyInMixToTargetVtHd[0].CleanUp();
	vtKeyInMixToTargetVtHd[1].CleanUp();
	fgKeyInTheOtherShellToFgHdInTarget[0].CleanUp();
	fgKeyInTheOtherShellToFgHdInTarget[1].CleanUp();
}

YsShellExtEdit_BooleanUtil::ConstEdgeOrigin::ConstEdgeOrigin()
{
	Initialize();
}

void YsShellExtEdit_BooleanUtil::ConstEdgeOrigin::Initialize(void)
{
	ceHd[0]=NULL;
	ceHd[1]=NULL;
}

////////////////////////////////////////////////////////////

void YsShellExtEdit_BooleanUtil::SetShellAndOpType(YsShellExtEdit &shlA,YsShellExtEdit &shlB,YSBOOLEANOPERATION boolOpType)
{
	this->shlA=&shlA;
	this->shlB=&shlB;
	this->boolOpType=boolOpType;

	usedVtxCache[0].SetShell((const YsShell &)shlA);
	usedVtxCache[1].SetShell((const YsShell &)shlB);

	usedVtxCache[0].CleanUp();
	usedVtxCache[1].CleanUp();

	for(int shlId=0; shlId<2; ++shlId)
	{
		const YsShellExtEdit &shl=(0==shlId ? shlA : shlB);
		for(auto vtHd : shl.AllVertex())
		{
			if(0<shl.GetNumPolygonUsingVertex(vtHd) || 0<shl.GetNumConstEdgeUsingVertex(vtHd))
			{
				usedVtxCache[shlId].AddVertex(vtHd);
			}
		}
	}
}


class LatticeSetDomainParam
{
public:
	const YsShell *shl;
	YsShellLattice *lat;
	int nLtcCell;
};

static void LatticeSetDomainCallBack(LatticeSetDomainParam *param)
{
	param->lat->SetDomain(*(param->shl),param->nLtcCell);
}

YSRESULT YsShellExtEdit_BooleanUtil::Run(void)
{
	mixed[0].CleanUp();
	mixed[1].CleanUp();

	const int nLtcCell=(shlA->GetNumPolygon()+shlB->GetNumPolygon())*10;
	YsShell tmpMix[2];

	YsDumpOldShellIntoNewShell(tmpMix[0],shlA->Conv(),0);
	YsDumpOldShellIntoNewShell(tmpMix[0],shlB->Conv(),1);


	YsShellLattice latA,latB,newShellLtc;
	// latA.SetDomain(shlA->Conv(),nLtcCell);
	// latB.SetDomain(shlB->Conv(),nLtcCell);
	// newShellLtc.SetDomain(tmpMix[0],nLtcCell);

	LatticeSetDomainParam paramA,paramB,paramNewShell;

	paramA.shl=(const YsShell *)shlA;
	paramA.lat=&latA;
	paramA.nLtcCell=nLtcCell;
	std::thread t1(LatticeSetDomainCallBack,&paramA);

	paramB.shl=(const YsShell *)shlB;
	paramB.lat=&latB;
	paramB.nLtcCell=nLtcCell;
	std::thread t2(LatticeSetDomainCallBack,&paramB);

	paramNewShell.shl=(const YsShell *)&tmpMix[0];
	paramNewShell.lat=&newShellLtc;
	paramNewShell.nLtcCell=nLtcCell;
	std::thread t3(LatticeSetDomainCallBack,&paramNewShell);

	t1.join();
	t2.join();
	t3.join();

	YSRESULT res=YsBlendShell3(tmpMix[0],tmpMix[1],newShellLtc,shlA->Conv(),latA,shlB->Conv(),latB,boolOpType);
	TransferBareMixToShellExt(0,tmpMix[0]);
	if(YSBOOLSEPARATE==boolOpType)
	{
		TransferBareMixToShellExt(1,tmpMix[1]);
	}

	return res;
}

void YsShellExtEdit_BooleanUtil::TransferBareMixToShellExt(int shlId,YsShell &bareMix)
{
// Polygon 
// freeAttribute2
// 	0: from shlA
// 	1: from shlB
// 	2: overlapping polygon of shlA and shlB
// freeAttribute3
// 	if freeAttribute2 is 0 or 1
// 		polygon index
// 	if freeAttribute2 is 2
// 		polygon index in shlA
// freeAttribute4
// 	if freeAttribute2 is 2
// 		polygon index in shlB
// 
// Vertex
// freeAttribute3
// 	0: from shlA
// 	1: from shlB
// 	Other: created
// freeAttribute4
// 	if freeAttribute3 is 0 or 1
// 		vertex index

	mixed[shlId].DisableSearch();

	shlA->Encache();
	shlB->Encache();

	vtKeyInMixToOrigin[shlId].CleanUp();
	plKeyInMixToOrigin[shlId].CleanUp();
	ceKeyInMixToOrigin[shlId].CleanUp();

	YsHashTable <YsShellVertexHandle> bareVtKeyToMixedVtHd;

	for(auto bareVtHd : bareMix.AllVertex())
	{
		const YsShellVertex *vertex=bareMix.GetVertex(bareVtHd);

		const YsVec3 vtPos=bareMix.GetVertexPosition(bareVtHd);
		auto vtHd=mixed[shlId].AddVertex(vtPos);

		bareVtKeyToMixedVtHd.AddElement(bareMix.GetSearchKey(bareVtHd),vtHd);

		VertexOrigin origin;
		switch(vertex->freeAttribute3)
		{
		case 0:
			origin.vtHd[0]=shlA->GetVertexHandleFromId(vertex->freeAttribute4);
			vtKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(vtHd),origin);
			break;
		case 1:
			origin.vtHd[1]=shlB->GetVertexHandleFromId(vertex->freeAttribute4);
			vtKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(vtHd),origin);
			break;
		default:
			break;
		}
	}

	int count=0;
	for(auto barePlHd : bareMix.AllPolygon())
	{
		const YsShellPolygon *polygon=bareMix.GetPolygon(barePlHd);

		YsArray <YsShellVertexHandle> plVtHd;
		bareMix.GetPolygon(plVtHd,barePlHd);

		for(auto &vtHd : plVtHd)
		{
			bareVtKeyToMixedVtHd.FindElement(vtHd,bareMix.GetSearchKey(vtHd));
		}

		auto plHd=mixed[shlId].AddPolygon(plVtHd);

		const YsVec3 nom=bareMix.GetNormal(barePlHd);
		mixed[shlId].SetPolygonNormal(plHd,nom);

		const YsColor col=bareMix.GetColor(barePlHd);
		mixed[shlId].SetPolygonColor(plHd,col);

		PolygonOrigin origin;
		switch(polygon->freeAttribute2)
		{
		case 0:
			origin.plHd[0]=shlA->GetPolygonHandleFromId(polygon->freeAttribute3);
			plKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(plHd),origin);
			break;
		case 1:
			origin.plHd[1]=shlB->GetPolygonHandleFromId(polygon->freeAttribute3);
			plKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(plHd),origin);
			break;
		case 2:
			origin.plHd[0]=shlA->GetPolygonHandleFromId(polygon->freeAttribute3);
			origin.plHd[1]=shlB->GetPolygonHandleFromId(polygon->freeAttribute4);
			plKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(plHd),origin);
			break;
		default:
			break;
		}

		bareMix.DeletePolygon(barePlHd); // Delete as it is transferred.

		++count;
		if(1024<=count)
		{
			count=0;
			bareMix.CollectGarbage();
		}
	}

	mixed[shlId].EnableSearch();
}

void YsShellExtEdit_BooleanUtil::MakeVertexMapping(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	vtKeyInMixToTargetVtHd[shlId].CleanUp();

	for(auto vtHd : mixed[shlId].AllVertex())
	{
		VertexOrigin origin;
		if(YSOK==vtKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(vtHd)) && NULL!=origin.vtHd[shlId])
		{
			vtKeyInMixToTargetVtHd[shlId].AddElement(mixed[shlId].GetSearchKey(vtHd),origin.vtHd[shlId]);
		}
		else
		{
			auto newVtHd=targetShl.AddVertex(mixed[shlId].GetVertexPosition(vtHd));
			vtKeyInMixToTargetVtHd[shlId].AddElement(mixed[shlId].GetSearchKey(vtHd),newVtHd);
			usedVtxCache[shlId].AddVertex(newVtHd);
		}
	}
}

void YsShellExtEdit_BooleanUtil::CacheEdgeBetweenShell(YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &edgeBetweenShl,int shlId)
{
	edgeBetweenShl.CleanUp();

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==mixed[shlId].MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		mixed[shlId].GetEdge(edVtHd,edHd);

		if(0<mixed[shlId].GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			// Don't be worried about already-constrained edge pieces.
			continue;
		}

		YSSIZE_T nPl;
		const YsShellPolygonHandle *edPlHd;
		if(YSOK==mixed[shlId].FindPolygonFromEdgePiece(nPl,edPlHd,edVtHd) && 1<nPl)
		{
			PolygonOrigin baseOrigin;
			plKeyInMixToOrigin[shlId].FindElement(baseOrigin,mixed[shlId].GetSearchKey(edPlHd[0]));

			YSBOOL allEqual=YSTRUE;
			for(int i=1; i<nPl; ++i)
			{
				PolygonOrigin checkOrigin;
				plKeyInMixToOrigin[shlId].FindElement(checkOrigin,mixed[shlId].GetSearchKey(edPlHd[i]));

				if(baseOrigin.GetCode()!=checkOrigin.GetCode())
				{
					allEqual=YSFALSE;
					break;
				}
			}

			if(YSTRUE!=allEqual)
			{
				YSHASHKEY key[1];
				key[0]=mixed[shlId].GetSearchKey(edVtHd[0]);
				edgeBetweenShl.AddElement(1,key,edVtHd[1]);
				key[0]=mixed[shlId].GetSearchKey(edVtHd[1]);
				edgeBetweenShl.AddElement(1,key,edVtHd[0]);
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::CacheUnusedPolygon(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	YsShellPolygonStore used((const YsShell &)targetShl);

	for(auto plHd : mixed[shlId].AllPolygon())
	{
		PolygonOrigin origin;
		if(YSOK==plKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(plHd)) && NULL!=origin.plHd[shlId])
		{
			used.AddPolygon(origin.plHd[shlId]);
		}
	}

	unusedPlKey[shlId].CleanUp();
	for(auto plHd : targetShl.AllPolygon())
	{
		if(YSTRUE!=used.IsIncluded(plHd))
		{
			unusedPlKey[shlId].Append(targetShl.GetSearchKey(plHd));
		}
	}
}

void YsShellExtEdit_BooleanUtil::TransferRemainingPartOfConstEdge(int shlId)
{
	YsHashTable <YsShellVertexHandle> srcVtKeyToMixed[2];

	for(auto vtHd : mixed[shlId].AllVertex())
	{
		VertexOrigin origin;
		if(YSOK==vtKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(vtHd)))
		{
			if(origin.vtHd[0]!=NULL)
			{
				srcVtKeyToMixed[0].AddElement(shlA->GetSearchKey(origin.vtHd[0]),vtHd);
			}
			if(origin.vtHd[1]!=NULL)
			{
				srcVtKeyToMixed[1].AddElement(shlB->GetSearchKey(origin.vtHd[1]),vtHd);
			}
		}
	}

	for(int srcShlIdx=0; srcShlIdx<2; ++srcShlIdx)
	{
		const auto &srcShl=(0==srcShlIdx ? *shlA : *shlB);
		for(auto ceHd : srcShl.AllConstEdge())
		{
			YSBOOL isLoop;
			YsArray <YsShellVertexHandle> srcCeVtHd;
			srcShl.GetConstEdge(srcCeVtHd,isLoop,ceHd);
			if(0<srcCeVtHd.GetN())
			{
				if(YSTRUE==isLoop)
				{
					auto vtHd0=srcCeVtHd[0];
					srcCeVtHd.Append(vtHd0);
				}
			}

			YsArray <YsShellVertexHandle> newCeVtHd;

			for(YSSIZE_T idx=0; idx<srcCeVtHd.GetN(); ++idx)
			{
				YsShellVertexHandle vtHd;
				if(YSOK==srcVtKeyToMixed[srcShlIdx].FindElement(vtHd,srcShl.GetSearchKey(srcCeVtHd[idx])))
				{
					newCeVtHd.Append(vtHd);
				}
				else
				{
					newCeVtHd.Append(NULL);
				}
			}

			YsArray <YsShellVertexHandle> remainingCeVtHd;
			if(NULL!=newCeVtHd[0])  // 2014/10/16 This check was missing.  If the first vertex of the const edge did not survive, NULL was added in the remainingCeVtHd, which caused the crash in the downstream.
			{
				remainingCeVtHd.Append(newCeVtHd[0]);
			}
			for(YSSIZE_T i=1; i<srcCeVtHd.GetN(); ++i)
			{
				if(NULL==newCeVtHd[i]) // It's cut anyway.
				{
					if(2<=remainingCeVtHd.GetN())
					{
						auto ceHdInMix=mixed[shlId].AddConstEdge(remainingCeVtHd,YSFALSE);
						ConstEdgeOrigin origin;
						origin.ceHd[srcShlIdx]=ceHd;
						ceKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(ceHdInMix),origin);

						auto attrib=srcShl.GetConstEdgeAttrib(ceHd);
						mixed[shlId].SetConstEdgeAttrib(ceHdInMix,attrib);
					}
					remainingCeVtHd.CleanUp();
				}
				else
				{
					if(0==remainingCeVtHd.GetN())
					{
						remainingCeVtHd.Append(newCeVtHd[i]);
					}
					else if(0==srcShl.GetNumPolygonUsingEdge(srcCeVtHd[i-1],srcCeVtHd[i])) // If it is not used, don't care.
					{
						remainingCeVtHd.Append(newCeVtHd[i]);
					}
					else 
					{
						// So, the original const-edge piece was used by a polygon.  What about in the mixed shell?
						if(0<mixed[shlId].GetNumPolygonUsingEdge(newCeVtHd[i-1],newCeVtHd[i])) // If this edge piece survived, it is safe to re-constrain.
						{
							remainingCeVtHd.Append(newCeVtHd[i]);
						}
						else
						{
							// Oops!  It's cut.
							if(2<=remainingCeVtHd.GetN())
							{
								auto ceHdInMix=mixed[shlId].AddConstEdge(remainingCeVtHd,YSFALSE);
								ConstEdgeOrigin origin;
								origin.ceHd[srcShlIdx]=ceHd;
								ceKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(ceHdInMix),origin);

								auto attrib=srcShl.GetConstEdgeAttrib(ceHd);
								mixed[shlId].SetConstEdgeAttrib(ceHdInMix,attrib);
							}
							remainingCeVtHd.CleanUp();
						}
					}
				}
			}

			if(remainingCeVtHd.GetN()==srcCeVtHd.GetN()) // Means entire const-edge survived.
			{
				if(YSTRUE==isLoop)
				{
					remainingCeVtHd.DeleteLast();
				}
				if(2<=remainingCeVtHd.GetN())
				{
					auto ceHdInMix=mixed[shlId].AddConstEdge(remainingCeVtHd,isLoop);
					ConstEdgeOrigin origin;
					origin.ceHd[srcShlIdx]=ceHd;
					ceKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(ceHdInMix),origin);

					auto attrib=srcShl.GetConstEdgeAttrib(ceHd);
					mixed[shlId].SetConstEdgeAttrib(ceHdInMix,attrib);
				}
			}
			else if(2<=remainingCeVtHd.GetN())
			{
				auto ceHdInMix=mixed[shlId].AddConstEdge(remainingCeVtHd,YSFALSE);
				ConstEdgeOrigin origin;
				origin.ceHd[srcShlIdx]=ceHd;
				ceKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(ceHdInMix),origin);
				remainingCeVtHd.CleanUp();

				auto attrib=srcShl.GetConstEdgeAttrib(ceHd);
				mixed[shlId].SetConstEdgeAttrib(ceHdInMix,attrib);
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::ReconstructConstEdge(int shlId)
{
	for(int srcShlIdx=0; srcShlIdx<2; ++srcShlIdx)
	{
		const auto &srcShl=(0==srcShlIdx ? *shlA : *shlB);

		// Make polygon-pair to ceHd table.
		YsFixedLengthToMultiHashTable <YsShellExt::ConstEdgeHandle,2,1> srcPlgPairToSrcCeHd;

		for(YsShellEdgeEnumHandle edHd=NULL; YSOK==srcShl.MoveToNextEdge(edHd); )
		{
			YsShellVertexHandle edVtHd[2];
			srcShl.GetEdge(edVtHd,edHd);

			YsArray <YsShellExt::ConstEdgeHandle> edCeHd;
			if(YSOK==srcShl.FindConstEdgeFromEdgePiece(edCeHd,edVtHd) && 0<edCeHd.GetN())
			{
				YsArray <YsShellPolygonHandle,2> edPlHd;
				srcShl.FindPolygonFromEdgePiece(edPlHd,edVtHd);

				for(int i=0; i<edPlHd.GetN(); ++i)
				{
					for(int j=i+1; j<edPlHd.GetN(); ++j)
					{
						YSHASHKEY edPlKey[2]={srcShl.GetSearchKey(edPlHd[i]),srcShl.GetSearchKey(edPlHd[j])};
						srcPlgPairToSrcCeHd.AddElement(2,edPlKey,edCeHd[0]);
					}
				}
			}
		}

		// Now constrain whatever missing edges based on origin plHd in the mix
		for(YsShellEdgeEnumHandle edHd=NULL; YSOK==mixed[shlId].MoveToNextEdge(edHd); )
		{
			YsShellVertexHandle edVtHd[2];
			mixed[shlId].GetEdge(edVtHd,edHd);
			if(0==mixed[shlId].GetNumConstEdgeUsingEdgePiece(edVtHd))
			{
				YsArray <YsShellPolygonHandle,2> edPlHdInMix;
				mixed[shlId].FindPolygonFromEdgePiece(edPlHdInMix,edVtHd);
				if(2<=edPlHdInMix.GetN())
				{
					PolygonOrigin origin[2];
					if(YSOK==plKeyInMixToOrigin[shlId].FindElement(origin[0],mixed[shlId].GetSearchKey(edPlHdInMix[0])) &&
					   NULL!=origin[0].plHd[srcShlIdx] &&
					   YSOK==plKeyInMixToOrigin[shlId].FindElement(origin[1],mixed[shlId].GetSearchKey(edPlHdInMix[1])) &&
					   NULL!=origin[1].plHd[srcShlIdx])
					{
						const YSHASHKEY srcPlKey[2]=
						{
							srcShl.GetSearchKey(origin[0].plHd[srcShlIdx]),
							srcShl.GetSearchKey(origin[1].plHd[srcShlIdx])
						};
						int nCe;
						YsShellExt::ConstEdgeHandle *ceHd;
						if(YSOK==srcPlgPairToSrcCeHd.FindElement(nCe,ceHd,2,srcPlKey) && 0<nCe)
						{
							auto newCeHd=mixed[shlId].AddConstEdge(2,edVtHd,YSFALSE);
							ConstEdgeOrigin origin;
							origin.ceHd[srcShlIdx]=ceHd[0];
							ceKeyInMixToOrigin[shlId].AddElement(mixed[shlId].GetSearchKey(newCeHd),origin);
						}
					}
				}
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::CombineConstEdgeOfSameOrigin(int shlId)
{
	for(auto vtHd : mixed[shlId].AllVertex())
	{
		YsArray <YsShellExt::ConstEdgeHandle> vtCeHd;

		if(YSOK==mixed[shlId].FindConstEdgeFromVertex(vtCeHd,vtHd) && vtCeHd.GetN()==2 && vtCeHd[0]!=vtCeHd[1])
		{
			ConstEdgeOrigin origin[2];
			ceKeyInMixToOrigin[shlId].FindElement(origin[0],mixed[shlId].GetSearchKey(vtCeHd[0]));
			ceKeyInMixToOrigin[shlId].FindElement(origin[1],mixed[shlId].GetSearchKey(vtCeHd[1]));

			if(origin[0].ceHd[0]==origin[1].ceHd[0] && origin[0].ceHd[1]==origin[1].ceHd[1])
			{
				YSBOOL srcIsLoop=YSFALSE;
				if(NULL!=origin[0].ceHd[0])
				{
					YsArray <YsShellVertexHandle> tmpCeVtHd;
					shlA->GetConstEdge(tmpCeVtHd,srcIsLoop,origin[0].ceHd[0]);
				}
				else if(NULL!=origin[0].ceHd[1])
				{
					YsArray <YsShellVertexHandle> tmpCeVtHd;
					shlA->GetConstEdge(tmpCeVtHd,srcIsLoop,origin[0].ceHd[1]);
				}


				YSBOOL isLoop[2];
				YsArray <YsShellVertexHandle> ceVtHd[2],newCeVtHd;

				mixed[shlId].GetConstEdge(ceVtHd[0],isLoop[0],vtCeHd[0]);
				mixed[shlId].GetConstEdge(ceVtHd[1],isLoop[1],vtCeHd[1]);

				if(YSFALSE==isLoop[0] && YSFALSE==isLoop[1])
				{
					if(ceVtHd[0].Last()==ceVtHd[1][0])
					{
						ceVtHd[0].DeleteLast();
						newCeVtHd=ceVtHd[0];
						newCeVtHd.Append(ceVtHd[1]);
					}
					else if(ceVtHd[0].Last()==ceVtHd[1].Last())
					{
						ceVtHd[0].DeleteLast();
						ceVtHd[1].Invert();
						newCeVtHd=ceVtHd[0];
						newCeVtHd.Append(ceVtHd[1]);
					}
					else if(ceVtHd[0][0]==ceVtHd[1][0])
					{
						ceVtHd[0].Invert();
						ceVtHd[0].DeleteLast();
						newCeVtHd=ceVtHd[0];
						newCeVtHd.Append(ceVtHd[1]);
					}
					else if(ceVtHd[0][0]==ceVtHd[1].Last())
					{
						ceVtHd[1].DeleteLast();
						newCeVtHd=ceVtHd[1];
						newCeVtHd.Append(ceVtHd[0]);
					}

					if(2<=newCeVtHd.GetN())
					{
						YSBOOL isLoop=YSFALSE;
						if(YSTRUE==srcIsLoop && newCeVtHd[0]==newCeVtHd.Last())
						{
							isLoop=YSTRUE;
							newCeVtHd.DeleteLast();
						}
						mixed[shlId].ModifyConstEdge(vtCeHd[0],newCeVtHd,isLoop);
						mixed[shlId].DeleteConstEdge(vtCeHd[1]);
					}
				}
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::ConstrainBetweenInputShell(int shlId)
{
	YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> edgeBetweenShl;

	CacheEdgeBetweenShell(edgeBetweenShl,shlId);

	for(auto vtHd : mixed[shlId].AllVertex())
	{
		const YSHASHKEY key[1]={mixed[shlId].GetSearchKey(vtHd)};
		int nConn;
		const YsShellVertexHandle *connVtHdArray;
		if(YSOK==edgeBetweenShl.FindElement(nConn,connVtHdArray,1,key))
		{
			for(int conn=0; conn<nConn; ++conn)
			{
				YsShellVertexHandle connVtHd=connVtHdArray[conn];
				if(0==mixed[shlId].GetNumConstEdgeUsingEdgePiece(vtHd,connVtHd))
				{
					YsArray <YsShellVertexHandle> vtHdTrack=YsShellExt_TrackingUtil::TrackVertexConnection((const YsShell &)mixed[shlId],vtHd,connVtHd,edgeBetweenShl);
					if(2<=vtHdTrack.GetN())
					{
						YSBOOL isLoop=YSFALSE;
						if(vtHdTrack[0]==vtHdTrack.Last())
						{
							isLoop=YSTRUE;
							vtHdTrack.DeleteLast();
						}

						if(2<=vtHdTrack.GetN())
						{
							mixed[shlId].AddConstEdge(vtHdTrack,isLoop);
						}
					}
				}
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::DeleteTemporaryVertex(int shlId)
{
	// See memo/boolean.jnt
	for(auto vtHd : mixed[shlId].AllVertex())
	{
		// Condition 3: No vertex origin
		VertexOrigin vtOrigin;
		if(YSOK==vtKeyInMixToOrigin[shlId].FindElement(vtOrigin,mixed[shlId].GetSearchKey(vtHd)) &&
		   (NULL!=vtOrigin.vtHd[0] || NULL!=vtOrigin.vtHd[1]))
		{
			continue;
		}


		// Condition 1: Used from two original polygons
		YsArray <YsShellPolygonHandle> vtPlHd;
		mixed[shlId].FindPolygonFromVertex(vtPlHd,vtHd);

		YsArray <PolygonOrigin> plOrigin=PlOriginOfMultiPolygon(shlId,vtPlHd);

		if(2!=plOrigin.GetN())
		{
			continue;
		}

		// Condition 3: Two possible collapse direction and they are on a straight line.
		YsArray <YsShellVertexHandle,16> connVtHdArray;
		YsArray <YsShellVertexHandle,2> collapsible;
		if(YSOK==mixed[shlId].GetConnectedVertex(connVtHdArray,vtHd))
		{
			for(auto connVtHd : connVtHdArray)
			{
				YsArray <YsShellPolygonHandle,2> edPlHd;
				mixed[shlId].FindPolygonFromEdgePiece(edPlHd,vtHd,connVtHd);

				YsArray <PolygonOrigin> plOrigin=PlOriginOfMultiPolygon(shlId,edPlHd);
				if(2==plOrigin.GetN())
				{
					collapsible.Append(connVtHd);
				}
			}
		}

		if(2!=collapsible.GetN())
		{
			continue;
		}


		// All 3 conditions have been satisified.

		// Debug ou
		YsPrintf("Collapsible [0] %s to %s\n",mixed[shlId].GetVertexPosition(vtHd).Txt(),mixed[shlId].GetVertexPosition(collapsible[0]).Txt());
		YsPrintf("Collapsible [1] %s to %s\n",mixed[shlId].GetVertexPosition(vtHd).Txt(),mixed[shlId].GetVertexPosition(collapsible[1]).Txt());

		YsShell_CollapseInfo collapseUtil;
		if(YSOK==collapseUtil.MakeInfo(mixed[shlId],vtHd,collapsible[0]))
		{
			for(YSSIZE_T idx=0; idx<collapseUtil.newPlg.GetN(); ++idx)
			{
				auto &plgChange=collapseUtil.newPlg[idx];
				mixed[shlId].SetPolygonVertex(plgChange.plHd,plgChange.plVtHd);
			}
			for(YSSIZE_T idx=0; idx<collapseUtil.newConstEdge.GetN(); ++idx)
			{
				auto &ceChange=collapseUtil.newConstEdge[idx];
				mixed[shlId].ModifyConstEdge(ceChange.ceHd,ceChange.ceVtHd,ceChange.isLoop);
			}
			for(auto plHd : collapseUtil.plHdToDel)
			{
				mixed[shlId].DeletePolygon(plHd);
			}
			for(auto ceHd : collapseUtil.ceHdToDel)
			{
				mixed[shlId].DeleteConstEdge(ceHd);
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(int shlId)
{
	for(auto vtHd : mixed[shlId].AllVertex())
	{
		YsArray <YsShellPolygonHandle> vtPlHd;
		mixed[shlId].FindPolygonFromVertex(vtPlHd,vtHd);

		YsArray <PolygonOrigin> plOrigin=PlOriginOfMultiPolygon(shlId,vtPlHd);
		for(auto &origin : plOrigin)
		{
			MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(shlId,vtHd,origin);
		}
	}
}

void YsShellExtEdit_BooleanUtil::MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(
	int shlId,YsShellVertexHandle vtHd,YsShellExtEdit_BooleanUtil::PolygonOrigin &origin)
{
	YsArray <YsShellVertexHandle> connVtHdArray;
	mixed[shlId].GetConnectedVertex(connVtHdArray,vtHd);

	YsArray <YsShellVertexHandle> sameOriginConnVtHdArray;
	YsArray <double> sameOriginConnVtHdLength;
	for(auto connVtHd : connVtHdArray)
	{
		YsArray <YsShellPolygonHandle> edPlHdAll;
		mixed[shlId].FindPolygonFromEdgePiece(edPlHdAll,vtHd,connVtHd);

		PolygonOrigin edPlOrigin[2];
		if(edPlHdAll.GetN()!=2 ||
		   YSOK!=plKeyInMixToOrigin[shlId].FindElement(edPlOrigin[0],mixed[shlId].GetSearchKey(edPlHdAll[0])) ||
		   YSOK!=plKeyInMixToOrigin[shlId].FindElement(edPlOrigin[1],mixed[shlId].GetSearchKey(edPlHdAll[1])) ||
		   origin!=edPlOrigin[0] ||
		   origin!=edPlOrigin[1])
		{
			continue;
		}
		sameOriginConnVtHdArray.Append(connVtHd);
		sameOriginConnVtHdLength.Append(mixed[shlId].GetEdgeLength(vtHd,connVtHd));
	}

	if(1<sameOriginConnVtHdArray.GetN())
	{
		YsQuickSort <double,YsShellVertexHandle> (sameOriginConnVtHdLength.GetN(),sameOriginConnVtHdLength,sameOriginConnVtHdArray);
		MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(shlId,vtHd,sameOriginConnVtHdArray);
	}
}

void YsShellExtEdit_BooleanUtil::MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(int shlId,YsShellVertexHandle vtHd,const YsArray <YsShellVertexHandle> &sortedConnVtHdArray)
{
	for(YSSIZE_T idx=sortedConnVtHdArray.GetN()-1; 0<idx; --idx)
	{
		YsShell_MergeInfo mergeInfo;
		YsArray <YsShellPolygonHandle,2> edPlHdAll;

		mixed[shlId].FindPolygonFromEdgePiece(edPlHdAll,vtHd,sortedConnVtHdArray[idx]);
		if(2==edPlHdAll.GetN())
		{
			mergeInfo.CleanUp();
			if(YSOK==mergeInfo.MakeInfo((const YsShell &)mixed[shlId],edPlHdAll) && YSTRUE!=mergeInfo.duplicateWarning)
			{
				mixed[shlId].SetPolygonVertex(edPlHdAll[0],mergeInfo.newPlVtHd);
				mixed[shlId].DeletePolygon(edPlHdAll[1]);
			}
		}
	}
}

YsArray <YsShellExtEdit_BooleanUtil::PolygonOrigin> YsShellExtEdit_BooleanUtil::PlOriginOfMultiPolygon(int shlId,const YsArray <YsShellPolygonHandle> &plHdArray)
{
	YsArray <YsShellExtEdit_BooleanUtil::PolygonOrigin> plOrigin;
	for(auto plHd : plHdArray)
	{
		PolygonOrigin o;
		if(YSOK==plKeyInMixToOrigin[shlId].FindElement(o,mixed[shlId].GetSearchKey(plHd)))
		{
			if((NULL!=o.plHd[0] || NULL!=o.plHd[1]) &&YSTRUE!=plOrigin.IsIncluded(o))
			{
				plOrigin.Append(o);
			}
		}
	}
	return plOrigin;
}

void YsShellExtEdit_BooleanUtil::ModifyShell(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	YsFixedLengthToMultiHashTable <YsShellPolygonHandle,1,1> targetPlKeyToMixedPlHd;

	for(auto plHd : mixed[shlId].AllPolygon())
	{
		PolygonOrigin origin;
		if(YSOK==plKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(plHd)) && NULL!=origin.plHd[shlId])
		{
			unsigned int key[1]={targetShl.GetSearchKey(origin.plHd[shlId])};
			targetPlKeyToMixedPlHd.AddElement(1,key,plHd);
		}
	}

	auto allPlHd=targetShl.AllPolygon().Array();
	for(auto plHdInTarget : allPlHd)
	{
		unsigned int key[1]={targetShl.GetSearchKey(plHdInTarget)};
		const YsArray <YsShellPolygonHandle> *plHdInMix;
		plHdInMix=targetPlKeyToMixedPlHd.FindElement(1,key);
		if(NULL!=plHdInMix && 0<plHdInMix->GetN())
		{
			if(1==plHdInMix->GetN())
			{
				YsArray <YsShellVertexHandle> newPlVtHd;
				mixed[shlId].GetPolygon(newPlVtHd,(*plHdInMix)[0]);
				for(auto &vtHd : newPlVtHd)
				{
					vtKeyInMixToTargetVtHd[shlId].FindElement(vtHd,mixed[shlId].GetSearchKey(vtHd));
				}

				YsArray <YsShellVertexHandle> curPlVtHd;
				targetShl.GetPolygon(curPlVtHd,plHdInTarget);

				YSBOOL diff=YSFALSE;
				if(curPlVtHd.GetN()!=newPlVtHd.GetN())
				{
					diff=YSTRUE;
				}
				else
				{
					for(int vtIdx=0; vtIdx<curPlVtHd.GetN(); ++vtIdx)
					{
						if(curPlVtHd[vtIdx]!=newPlVtHd[vtIdx])
						{
							diff=YSTRUE;
							break;
						}
					}
				}

				if(YSTRUE==diff)
				{
					targetShl.SetPolygonVertex(plHdInTarget,newPlVtHd);
				}

				YsVec3 newNom;
				mixed[shlId].GetNormal(newNom,(*plHdInMix)[0]);

				YsVec3 curNom;
				targetShl.GetNormal(curNom,plHdInTarget);
				if(newNom!=curNom)
				{
					targetShl.SetPolygonNormal(plHdInTarget,newNom);
				}
			}
			else
			{
				for(auto srcPlHd : (*plHdInMix))
				{
					YsArray <YsShellVertexHandle> newPlVtHd;
					mixed[shlId].GetPolygon(newPlVtHd,srcPlHd);
					for(auto &vtHd : newPlVtHd)
					{
						vtKeyInMixToTargetVtHd[shlId].FindElement(vtHd,mixed[shlId].GetSearchKey(vtHd));
					}

					if(srcPlHd==(*plHdInMix)[0])
					{
						targetShl.SetPolygonVertex(plHdInTarget,newPlVtHd);
					}
					else
					{
						YsShellPolygonHandle newPlHd=targetShl.AddPolygon(newPlVtHd);

						const YsShellExt::PolygonAttrib *attrib=targetShl.GetPolygonAttrib(plHdInTarget);
						targetShl.SetPolygonAttrib(newPlHd,*attrib);

						YsShellExt::FaceGroupHandle fgHd=targetShl.FindFaceGroupFromPolygon(plHdInTarget);
						if(NULL!=fgHd)
						{
							targetShl.AddFaceGroupPolygon(fgHd,1,&newPlHd);
						}

						YsColor col;
						targetShl.GetColor(col,plHdInTarget);
						targetShl.SetPolygonColor(newPlHd,col);

						YsVec3 nom;
						mixed[shlId].GetNormal(nom,srcPlHd);
						targetShl.SetPolygonNormal(newPlHd,nom);
					}
				}
			}
		}
	}
}

void YsShellExtEdit_BooleanUtil::AddPolygonFromTheOtherShell(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);
	YsShellExtEdit &theOtherShl=(0==shlId ? *shlB : *shlA);

	fgKeyInTheOtherShellToFgHdInTarget[shlId].CleanUp();

	for(auto mixedPlHd : mixed[shlId].AllPolygon())
	{
		PolygonOrigin origin;
		if(YSOK==plKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(mixedPlHd)) && NULL==origin.plHd[shlId] && NULL!=origin.plHd[1-shlId])
		{
			YsArray <YsShellVertexHandle> plVtHd;
			mixed[shlId].GetPolygon(plVtHd,mixedPlHd);

			for(auto &vtHd : plVtHd)
			{
				vtKeyInMixToTargetVtHd[shlId].FindElement(vtHd,mixed[shlId].GetSearchKey(vtHd));
			}

			YsShellPolygonHandle newPlHd=targetShl.AddPolygon(plVtHd);

			YsShellPolygonHandle plHdInTheOtherShl=origin.plHd[1-shlId];

			const YsShellExt::PolygonAttrib *attrib=theOtherShl.GetPolygonAttrib(plHdInTheOtherShl);
			targetShl.SetPolygonAttrib(newPlHd,*attrib);

			YsShellExt::FaceGroupHandle fgHdInTheOtherShl=theOtherShl.FindFaceGroupFromPolygon(plHdInTheOtherShl);
			if(NULL!=fgHdInTheOtherShl)
			{
				YsShellExt::FaceGroupHandle fgHdInTarget=NULL;
				if(YSOK==fgKeyInTheOtherShellToFgHdInTarget[shlId].FindElement(fgHdInTarget,theOtherShl.GetSearchKey(fgHdInTheOtherShl)) && NULL!=fgHdInTheOtherShl)
				{
					targetShl.AddFaceGroupPolygon(fgHdInTarget,1,&newPlHd);
				}
				else
				{
					fgHdInTarget=targetShl.AddFaceGroup(1,&newPlHd);
					fgKeyInTheOtherShellToFgHdInTarget[shlId].UpdateElement(theOtherShl.GetSearchKey(fgHdInTheOtherShl),fgHdInTarget);

					YsShellExt::FaceGroupAttrib attrib=theOtherShl.GetFaceGroupAttrib(fgHdInTheOtherShl);
					targetShl.SetFaceGroupAttrib(fgHdInTarget,attrib);
				}
			}

			YsColor col;
			theOtherShl.GetColor(col,plHdInTheOtherShl);
			targetShl.SetPolygonColor(newPlHd,col);

			YsVec3 nom;
			mixed[shlId].GetNormal(nom,mixedPlHd);
			targetShl.SetPolygonNormal(newPlHd,nom);
		}
	}
}

void YsShellExtEdit_BooleanUtil::ApplyConstEdgeChange(int shlId)
{
	// mixed[shlId] to shlA(shlId==0) or shlB(shlId==1)
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	YsKeyStore usedCeKey;

	for(auto ceHdInMix : mixed[shlId].AllConstEdge())
	{
		YSBOOL isLoop;
		YsArray <YsShellVertexHandle> ceVtHd;

		mixed[shlId].GetConstEdge(ceVtHd,isLoop,ceHdInMix);

		YSBOOL bailOut=YSFALSE;

		for(auto &vtHd : ceVtHd)
		{
			if(YSOK!=vtKeyInMixToTargetVtHd[shlId].FindElement(vtHd,mixed[shlId].GetSearchKey(vtHd)) || NULL==vtHd)
			{
				bailOut=YSTRUE;
				break;
			}
		}

		if(YSTRUE==bailOut)
		{
			continue;
		}

		ConstEdgeOrigin origin;
		if(YSOK==ceKeyInMixToOrigin[shlId].FindElement(origin,mixed[shlId].GetSearchKey(ceHdInMix)) && NULL!=origin.ceHd[shlId])
		{
			// Means this origin.ceHd[shlId] survived.
			if(YSTRUE==usedCeKey.IsIncluded(targetShl.GetSearchKey(origin.ceHd[shlId])))
			{
				// The original ce has been divided into more than one segment, and the first one was already transformed.
				auto ceHdInTarget=targetShl.AddConstEdge(ceVtHd,isLoop);
				auto attrib=mixed[shlId].GetConstEdgeAttrib(ceHdInMix);
				targetShl.SetConstEdgeAttrib(ceHdInTarget,attrib);
			}
			else
			{
				usedCeKey.AddKey(targetShl.GetSearchKey(origin.ceHd[shlId]));

				YSBOOL curIsLoop;
				YsArray <YsShellVertexHandle> curCeVtHd;
				targetShl.GetConstEdge(curCeVtHd,curIsLoop,origin.ceHd[shlId]);

				YSBOOL allEqual=YSTRUE;
				if(curIsLoop!=isLoop)
				{
					allEqual=YSFALSE;
				}
				else if(curCeVtHd.GetN()!=ceVtHd.GetN())
				{
					allEqual=YSFALSE;
				}
				else
				{
					for(int i=0; i<curCeVtHd.GetN(); ++i)
					{
						if(curCeVtHd[i]!=ceVtHd[i])
						{
							allEqual=YSFALSE;
							break;
						}
					}
				}
				if(YSTRUE!=allEqual)
				{
					targetShl.ModifyConstEdge(origin.ceHd[shlId],ceVtHd,isLoop);
				}
			}
		}
		else
		{
			// Means this is a new const edge between input shells, or coming from the other shell.
			auto ceHdInTarget=targetShl.AddConstEdge(ceVtHd,isLoop);
			auto attrib=mixed[shlId].GetConstEdgeAttrib(ceHdInMix);
			targetShl.SetConstEdgeAttrib(ceHdInTarget,attrib);

			usedCeKey.AddKey(targetShl.GetSearchKey(ceHdInTarget));
		}
	}

	for(auto ceHd : targetShl.AllConstEdge())
	{
		if(YSTRUE!=usedCeKey.IsIncluded(targetShl.GetSearchKey(ceHd)))
		{
			targetShl.DeleteConstEdge(ceHd);
		}
	}
}

void YsShellExtEdit_BooleanUtil::DeleteCachedUnusedPolygon(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	for(auto plKey : unusedPlKey[shlId])
	{
		auto plHd=targetShl.FindPolygon(plKey);
		if(NULL!=plHd)
		{
			YsArray <YsShellVertexHandle> plVtHd;
			targetShl.GetPolygon(plVtHd,plHd);

			targetShl.DeleteFaceGroupPolygon(plHd);
			targetShl.DeletePolygon(plHd);
		}
	}
}

void YsShellExtEdit_BooleanUtil::DeleteUsedVertexTurnedUnused(int shlId)
{
	YsShellExtEdit &targetShl=(0==shlId ? *shlA : *shlB);

	for(auto vtHd : usedVtxCache[shlId])
	{
		if(NULL!=vtHd &&
		   0==targetShl.GetNumPolygonUsingVertex(vtHd) && 
		   0==targetShl.GetNumConstEdgeUsingVertex(vtHd))
		{
			targetShl.DeleteVertex(vtHd);
		}
	}
}

