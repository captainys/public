/* ////////////////////////////////////////////////////////////

File Name: ysshellblend.cpp
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
#include "ysshell.h"


YSRESULT YsDumpOldShellIntoNewShell(YsShell &newShell,const YsShell &oldShell,int shlId)
{
	int baseVtId,i,nVtxPlg,plId,vtId;
	YsTemporaryObject <int> vtIdListSource;
	YsTemporaryObject <YsShellVertexHandle> newPlVtHdSource;
	int sufficientSize,*vtIdList;
	YsShellVertexHandle *newPlVtHd;
	YsShellPolygonHandle plHd;
	YsShellVertexHandle vtHd;
	YsArray <YsShellVertexHandle> vtHdMap;
	YsColor col;
	YsVec3 nom;

	oldShell.Encache();

	sufficientSize=oldShell.GetMaxNumVertexOfPolygon();
	newPlVtHd=newPlVtHdSource.GetArray(sufficientSize);
	vtIdList=vtIdListSource.GetArray(sufficientSize);
	baseVtId=newShell.GetNumVertex();

	vtHd=NULL;
	vtId=0;
	while((vtHd=oldShell.FindNextVertex(vtHd))!=NULL)
	{
		YsVec3 vtxPos;
		YsShellVertexHandle newVtHd;
		YsShellVertex *vtx;

		oldShell.GetVertexPosition(vtxPos,vtHd);
		newVtHd=newShell.AddVertexH(vtxPos);
		vtHdMap.Append(newVtHd);

		vtx=newShell.GetVertex(newVtHd);
		vtx->freeAttribute0=oldShell.GetVertex(vtHd)->freeAttribute0;
		vtx->freeAttribute1=oldShell.GetVertex(vtHd)->freeAttribute1;
		vtx->freeAttribute2=oldShell.GetVertex(vtHd)->freeAttribute2;
		vtx->freeAttribute3=shlId;
		vtx->freeAttribute4=vtId;

		vtId++;
	}

	plHd=NULL;
	plId=0;
	while((plHd=oldShell.FindNextPolygon(plHd))!=NULL)
	{
		nVtxPlg=oldShell.GetNumVertexOfPolygon(plHd);
		oldShell.GetVertexListOfPolygon(vtIdList,sufficientSize,plId);
		oldShell.GetColorOfPolygon(col,plHd);
		oldShell.GetNormalOfPolygon(nom,plHd);

		for(i=0; i<nVtxPlg; i++)
		{
			newPlVtHd[i]=vtHdMap[vtIdList[i]];
		}

		YsShellPolygonHandle newPlHd;
		YsShellPolygon *plg;
		newPlHd=newShell.AddPolygonH(nVtxPlg,newPlVtHd);

		newShell.SetColorOfPolygon(newPlHd,col);
		newShell.SetNormalOfPolygon(newPlHd,nom);

		plg=newShell.GetPolygon(newPlHd);
		plg->freeAttribute0=oldShell.GetPolygon(plHd)->freeAttribute0;
		plg->freeAttribute1=oldShell.GetPolygon(plHd)->freeAttribute1;
		plg->freeAttribute2=shlId;
		plg->freeAttribute3=plId;
		plg->freeAttribute4=0;   // At this point, I don't know if this is overlapping or not

		plId++;
	}

	return YSOK;
}

static YSRESULT YsGetShellPolygonVertexAndSearchKeyList
	(YsArray <YsVec3> &plgVtx,YsArray <unsigned> &plgVtxKey,const YsShell &shl,YsShellPolygonHandle plHd)
{
	int i,nVtHdPlg;
	const YsShellVertexHandle *vtHdPlg;

	nVtHdPlg=shl.GetNumVertexOfPolygon(plHd);
	vtHdPlg=shl.GetVertexListOfPolygon(plHd);

	plgVtx.Set(nVtHdPlg,NULL);
	plgVtxKey.Set(nVtHdPlg,NULL);
	for(i=0; i<nVtHdPlg; i++)
	{
		YsVec3 vtxPos;
		shl.GetVertexPosition(vtxPos,vtHdPlg[i]);

		plgVtx.SetItem(i,vtxPos);
		plgVtxKey.SetItem(i,shl.GetSearchKey(vtHdPlg[i]));
	}

	return YSOK;
}

// ######## Intensive Check 04/08/2000

static YsShellVertexHandle YsShellAddVertexOnEdge
    (YsShell &newShell,unsigned vtKey1,unsigned vtKey2,const YsVec3 &pos,const YsShellSearchTable &search)
{
	int nPlg;
	const YsShellPolygonHandle *plg;

	if(search.FindPolygonListByEdge(nPlg,plg,newShell,vtKey1,vtKey2)==YSOK && nPlg>0)
	{
		int i,j;
		YsShellVertexHandle newVtHd,vtHd1,vtHd2;
		int nPlVtHd;
		const YsShellVertexHandle *plVtHd;
		YsArray <YsShellPolygonHandle,2> plgList;
		YsArray <YsShellVertexHandle,256> modified;
		YsShellVertex *orgVtx,*neoVtx;

		vtHd1=search.FindVertex(newShell,vtKey1);
		vtHd2=search.FindVertex(newShell,vtKey2);
		orgVtx=newShell.GetVertex(vtHd1);

		plgList.Set(nPlg,plg);
		// Must make a copy. Because polygon list inside search table may change as a result of ModifyPolygon

		newVtHd=newShell.AddVertexH(pos);
		neoVtx=newShell.GetVertex(newVtHd);
		neoVtx->freeAttribute3=orgVtx->freeAttribute3;
		neoVtx->freeAttribute4=-1;  // New vertex

		for(i=0; i<plgList.GetNumItem(); i++)
		{
			YsShellPolygonHandle plHd;
			plHd=plgList.GetItem(i);

			nPlVtHd=newShell.GetNumVertexOfPolygon(plHd);
			plVtHd=newShell.GetVertexListOfPolygon(plHd);

			modified.Set(0,NULL);
			for(j=0; j<nPlVtHd; j++)
			{
				modified.AppendItem(plVtHd[j]);
				if((plVtHd[j]==vtHd1 && plVtHd[(j+1)%nPlVtHd]==vtHd2) ||
				   (plVtHd[j]==vtHd2 && plVtHd[(j+1)%nPlVtHd]==vtHd1))
				{
					modified.AppendItem(newVtHd);
				}
			}
			newShell.ModifyPolygon(plHd,modified.GetNumItem(),modified.GetArray());
		}
		return newVtHd;
	}
	else
	{
		// It is possible, for example, if the vertex is generated inside the slashed polygon
		// printf("Implies C-Loop\n");
		return newShell.AddVertexH(pos);
	}
}


YSRESULT YsSlashNewShellByOldShell
    (YsShell &newShell,int targetShlId,const YsShell &slasher,YSBOOL takeOverlap)
{
	int toBeSlashedIdx,slasherPlId;
	YsShellPolygonHandle slasherPlHd,newShellPlHd;
	YsShellPolygon *newShellPlg;
	YsArray <YsShellPolygonHandle> toBeSlashed;
	YsCollisionOfPolygon wizard;
	YsSword sword;
	YSBOOL slashed;
	const YsShellSearchTable *search;

	YsArray <YsVec3> plgVtx1,plgVtx2;
	YsArray <unsigned> plgVtxKey1,plgVtxKey2;


	search=newShell.GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsSlashNewShellByOldShell()\n");
		YsErrOut("  YsShellSearchTable must be attached before this process\n");
		return YSERR;
	}


	toBeSlashed.Set(0,NULL);
	newShellPlHd=NULL;
	while((newShellPlHd=newShell.FindNextPolygon(newShellPlHd))!=NULL)
	{
		newShellPlg=newShell.GetPolygon(newShellPlHd);
		if(newShellPlg->freeAttribute2==targetShlId)
		{
			toBeSlashed.AppendItem(newShellPlHd);
		}
	}

	for(toBeSlashedIdx=0; toBeSlashedIdx<toBeSlashed.GetNumItem(); toBeSlashedIdx++)
	{
		newShellPlHd=toBeSlashed.GetItem(toBeSlashedIdx);

		YsGetShellPolygonVertexAndSearchKeyList(plgVtx1,plgVtxKey1,newShell,newShellPlHd);
		wizard.SetPolygon1(plgVtx1.GetNumItem(),plgVtx1.GetArray());
		sword.SetInitialPolygon(plgVtx1.GetNumItem(),plgVtx1.GetArray(),(int *)plgVtxKey1.GetArray());

		slashed=YSFALSE;

		slasherPlHd=NULL;
		slasherPlId=0;
		while((slasherPlHd=slasher.FindNextPolygon(slasherPlHd))!=NULL)
		{
//			if(targetShlId==1)
//			   slasherPlId==0 &&
//			   newShell.GetPolygon(newShellPlHd)->freeAttribute3==14)
//			{
//				sword.debugMode=YSTRUE;
//			}
//			else
//			{
//				sword.debugMode=YSFALSE;
//			}

			YsGetShellPolygonVertexAndSearchKeyList(plgVtx2,plgVtxKey2,slasher,slasherPlHd);
			wizard.SetPolygon2(plgVtx2.GetNumItem(),plgVtx2.GetArray());

			if(wizard.CheckOnTheSamePlane()==YSTRUE)
			{
				sword.SlashByOverlappingPolygon3(plgVtx2.GetNumItem(),plgVtx2.GetArray(),slasherPlId);
				slashed=YSTRUE;
			}
			else if(wizard.CheckCollision()==YSTRUE)
			{
				sword.Slash(plgVtx2.GetNumItem(),plgVtx2.GetArray());
				slashed=YSTRUE;
			}
			else
			{
			}

			slasherPlId++;
		}

		if(slashed==YSTRUE)
		{
			YsColor col;
			YsVec3 nom;
			int freeAtt2,freeAtt3,freeAtt4;
			YsShellPolygon *plg;
			int i,j,n;


			// First, delete slashed polygon. This will be replaced by piecewise polygons.
			// But, remember color,normal and freeAttributes
			newShell.GetColorOfPolygon(col,newShellPlHd);
			newShell.GetNormalOfPolygon(nom,newShellPlHd);
			plg=newShell.GetPolygon(newShellPlHd);
			freeAtt2=plg->freeAttribute2;
			freeAtt3=plg->freeAttribute3;
			freeAtt4=plg->freeAttribute4;

			newShell.DeletePolygon(newShellPlHd);

			// Add new vertices. At the same time, update vertex searchKeys in YsSword.
			const YsSwordNewVertexLog *newVtxLog;
			sword.Encache();
			n=sword.GetNumNewVertex();
			for(i=0; i<n; i++)
			{
				YsShellVertexHandle newVtHd;
				unsigned newVtKey;

				newVtxLog=sword.GetNewVertexLog(i);
				newVtHd=YsShellAddVertexOnEdge(newShell,
				                          newVtxLog->betweenVt1,
				                          newVtxLog->betweenVt2,
				                          newVtxLog->pos,
				                          *search);
				newVtKey=newShell.GetSearchKey(newVtHd);

				sword.UpdateVertexId(newVtxLog->idCreated,newVtKey);
			}


			// Add polygons in YsSword, but no need to add new vertices
			const YsArray <int> *piecewise;
			YsArray <YsShellVertexHandle> newPlgVtHdList;
			n=sword.GetNumPolygon();
			for(i=0; i<n; i++)
			{
				piecewise=sword.GetVertexIdList(i);
				newPlgVtHdList.Set(piecewise->GetNumItem(),NULL);
				for(j=0; j<piecewise->GetNumItem(); j++)
				{
					YsShellVertexHandle vtHd;
					vtHd=search->FindVertex(newShell,piecewise->GetItem(j));
					newPlgVtHdList.SetItem(j,vtHd);
				}

				int slashedPlId;
				YsShellPolygonHandle plHd;
				YsShellPolygon *plNew;
				if(sword.IsFromOverlappingPolygon3(i,slashedPlId)!=YSTRUE)
				{
					plHd=newShell.AddPolygonH(newPlgVtHdList.GetNumItem(),newPlgVtHdList.GetArray());
					plNew=newShell.GetPolygon(plHd);
					plNew->SetColor(col);
					plNew->SetNormal(nom);
					plNew->freeAttribute2=freeAtt2;
					plNew->freeAttribute3=freeAtt3;
					plNew->freeAttribute4=freeAtt4;
				}
				else if(takeOverlap==YSTRUE)
				{
					plHd=newShell.AddPolygonH(newPlgVtHdList.GetNumItem(),newPlgVtHdList.GetArray());
					plNew=newShell.GetPolygon(plHd);
					plNew->SetColor(col);
					plNew->SetNormal(nom);
					plNew->freeAttribute2=2; //  2means shared polygon

					if(targetShlId==0)  // Means Slashing Shell[0] by Shell[1];
					{
						// Slashing Shell[0] by Shell[1] implies
						// freeAtt3 is PlId of Shell[0];
						plNew->freeAttribute3=freeAtt3;    // PlId of Shell[0];
						plNew->freeAttribute4=slashedPlId; // PlId of SHell[1];
					}
					else if(targetShlId==1)  // Means Slashing Shell[1] by Shell[0];
					{
						// Slashing Shell[0] by Shell[1] implies
						// freeAtt3 is PlId of Shell[1];
						plNew->freeAttribute3=slashedPlId; // PlId of SHell[1];
						plNew->freeAttribute4=freeAtt3;    // PlId of Shell[0];
					}
				}
			}
		}
	}

	return YSOK;
}

// Mix : Just running out of words. Want to say Blend
YSRESULT YsMixShell(YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	YSRESULT res;
	YsShell *mixer;
	YsShell tmp;

	if(&newShell==&sh1 || &newShell==&sh2)
	{
		mixer=&tmp;
	}
	else
	{
		mixer=&newShell;
	}

	YsShellSearchTable *search;

	if(mixer->GetSearchTable()==NULL)
	{
		search=new YsShellSearchTable(4096);
		mixer->AttachSearchTable(search);
	}
	else
	{
		search=NULL;
	}

	mixer->CleanUp();

	if(YsDumpOldShellIntoNewShell(*mixer,sh1,0)==YSOK &&
	   YsDumpOldShellIntoNewShell(*mixer,sh2,1)==YSOK &&
	   YsSlashNewShellByOldShell(*mixer,1,sh1,YSTRUE)==YSOK &&
	   YsSlashNewShellByOldShell(*mixer,0,sh2,YSFALSE)==YSOK)
	{
		res=YSOK;
	}
	else
	{
		res=YSERR;
	}

	if(search!=NULL)
	{
		mixer->DetachSearchTable();
		delete search;
	}

	if(mixer!=&newShell)
	{
		newShell=*mixer;
	}

	return res;
}

