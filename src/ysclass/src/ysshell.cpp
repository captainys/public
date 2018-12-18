/* ////////////////////////////////////////////////////////////

File Name: ysshell.cpp
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

#define YS_COMPILING_YSSHELL_CPP


#include "ysclass.h"
#include "ysshell.h"

#include <time.h>



/* virtual */ YSRESULT YsShell::PassAll::TestPolygon(const YsShell &,YsShell::PolygonHandle) const
{
	return YSOK;
}
/* virtual */ YSRESULT YsShell::PassAll::TestEdge(const YsShell &,YsShell::VertexHandle,YsShell::VertexHandle) const
{
	return YSOK;
}



// Implementation //////////////////////////////////////////
static int YsShellItemMemoryLeakTracker=0;

YsShellItem::YsShellItem()
{
	YsShellItemMemoryLeakTracker++;

	special=NULL;
	Initialize();
}

void YsShellItem::Initialize(void)
{
	freeAttribute0=0;
	freeAttribute1=0;
	freeAttribute2=0;
	freeAttribute3=0;
	freeAttribute4=0;
	special=NULL;
}

YsShellItem::~YsShellItem()
{
	YsShellItemMemoryLeakTracker--;

	if(YsShellItemMemoryLeakTracker==0)
	{
		YsDiagOut("YsShellItem class reports all allocated items are deleted now.\n");
	}
	else if(YsShellItemMemoryLeakTracker<0)
	{
		YsDiagOut("YsShellItem class reports items are deleted too many times.\n");
	}
}

void YsShellItem::CopyAttribute(const YsShellItem &from)
{
	freeAttribute0=from.freeAttribute0;
	freeAttribute1=from.freeAttribute1;
	freeAttribute2=from.freeAttribute2;
	freeAttribute3=from.freeAttribute3;
	freeAttribute4=from.freeAttribute4;
}


YSRESULT YsShellItem::SetSpecialAttribute(YsShellSpecialAttribute *attrib)
{
	special=attrib;
	return YSOK;
}

YsShellSpecialAttribute *YsShellItem::GetSpecialAttribute(void)
{
	return special;
}

const YsShellSpecialAttribute *YsShellItem::GetSpecialAttribute(void) const
{
	return special;
}

void YsShellItem::SpecialAttributeDetached(void)
{
	special=NULL;
}


// Implementation //////////////////////////////////////////
YsShellVertex::YsShellVertex()
{
	Initialize();
}

void YsShellVertex::Initialize(void)
{
	YsShellItem::Initialize();
	refCount=0;
}

YSHASHKEY YsShellVertex::GetSearchKey(void) const
{
	return searchKey;
}

YSRESULT YsShellVertex::IncrementReferenceCount(void)
{
	refCount++;
	return YSOK;
}

YSRESULT YsShellVertex::DecrementReferenceCount(void)
{
	refCount--;
	if(refCount<0)
	{
		YsErrOut(
		    "YsShellVertex::DecrementReferenceCount(void)\n"
		    "  Reference count became negative\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT YsShellVertex::SetPosition(const YsVec3 &p)
{
	pos=p;
	return YSOK;
}

YSRESULT YsShellVertex::SetNormal(const YsVec3 &n)
{
#ifndef YSLOWMEM
	nom=n;
#else
	YsErrOut("YsShellVertex::SetNormal()\n");
	YsErrOut("  Running light memory mode.\n");
	YsErrOut("  To use this function, link ysclass.lib instead of ysclasslm.lib\n");
#endif
	return YSOK;
}

const YsVec3 &YsShellVertex::GetPosition(void) const
{
	return pos;
}

const YsVec3 &YsShellVertex::GetNormal(void) const
{
#ifndef YSLOWMEM
	return nom;
#else
	YsErrOut("YsShellVertex::GetNormal()\n");
	YsErrOut("  Running light memory mode.\n");
	YsErrOut("  To use this function, link ysclass.lib instead of ysclasslm.lib\n");
	return YsOrigin();
#endif
}

int YsShellVertex::GetReferenceCount(void) const
{
	return refCount;
}

YSRESULT YsShellVertex::SetReferenceCount(int n)  // This function must be eliminated soon
{
	refCount=n;
	return YSOK;
}

YsShellVertexHandle YsShellVertex::Mapper(const class YsShell &owner) const
{
	return owner.FindVertexMapping(this);
}

void YsShellVertex::SetMapper(class YsShell &owner,YsShellVertexHandle toVtHd)
{
	owner.AddVertexMapping(this,toVtHd);
}

// Implementation //////////////////////////////////////////
YsShellPolygon::YsShellPolygon()
{
	Initialize();
}

void YsShellPolygon::Initialize(void)
{
	YsShellItem::Initialize();
	idx.Set(0,NULL);
	nom.Set(0.0,0.0,0.0);
#ifndef YSLOWMEM
	col.SetIntRGB(128,128,128);
#endif
}

YSRESULT YsShellPolygon::SetNormal(const YsVec3 &n)
{
	nom=n;
	return YSOK;
}

const YsVec3 &YsShellPolygon::GetNormal(void) const
{
	return nom;
}

YSRESULT YsShellPolygon::SetColor(const YsColor &c)
{
#ifndef YSLOWMEM
	col=c;
#endif
	return YSOK;
}

const YsColor &YsShellPolygon::GetColor(void) const
{
#ifndef YSLOWMEM
	return col;
#else
	return YsBlue();
#endif
}

YSRESULT YsShellPolygon::SetVertexList(const YsShell &shl,YSSIZE_T nVtId,const int vtId[])
{
	YSSIZE_T i;
	idx.Set(0,NULL);
	for(i=0; i<nVtId; i++)
	{
		idx.AppendItem(shl.GetVertexHandleFromId(vtId[i]));
	}
	return YSOK;
}

YSRESULT YsShellPolygon::SetVertexList(YSSIZE_T nVtx,const YsShellVertexHandle vtx[])
{
	idx.Set(nVtx,vtx);
	return YSOK;
}

int YsShellPolygon::GetNumVertex(void) const
{
	return (int)idx.GetNumItem(); // I don't think there is a polygon with more than 2 billion nodes.
}

const YsShellVertexHandle *YsShellPolygon::GetVertexArray(void) const
{
	return idx.GetArray();
}

int YsShellPolygon::GetVertexId(const YsShell &shl,YSSIZE_T i) const
{
	return shl.GetVertexIdFromHandle(idx[i]);
}

YsShellVertexHandle YsShellPolygon::GetVertexHandle(YSSIZE_T i) const
{
	return idx[i];
}

void YsShellPolygon::Invert(void)
{
	YSSIZE_T i,n;
	YsShellVertexHandle a,b;
	n=idx.GetNumItem();
	for(i=0; i<n/2; i++)
	{
		a=idx.GetItem(i);
		b=idx.GetItem(n-1-i);
		idx.SetItem(i,b);
		idx.SetItem(n-1-i,a);
	}
	nom=-nom;
}

////////////////////////////////////////////////////////////

YsShellReadSrfVariable::YsShellReadSrfVariable()
{
	mode=0;
	plHd=NULL;
	color.SetDoubleRGB(0.0,0.0,0.0);
	nom.Set(0.0,0.0,0.0);
	errorCount=0;
}

void YsShellReadSrfVariable::PrintErrorMessage(const char msg[])
{
	const int errorCountLimit=10;
	if(errorCountLimit>errorCount)
	{
		printf("%s\n",msg);
		++errorCount;
		if(errorCountLimit==errorCount)
		{
			printf("Error count reaches 10.  No more errors will be printed.\n");
		}
	}
}

// Implementation //////////////////////////////////////////



YsShell::YsShell()
{
	CommonInitialization();

	RewindSearchKey();

	bbxIsValid=YSFALSE;
	bbx1.Set(0.0,0.0,0.0);
	bbx2.Set(0.0,0.0,0.0);
}

YsShell::YsShell(const YsShell &from)
{
	CommonInitialization();
	CopyFrom(from);

	bbxIsValid=YSFALSE;
	bbx1.Set(0.0,0.0,0.0);
	bbx2.Set(0.0,0.0,0.0);
}

void YsShell::CommonInitialization(void)
{
	vtx.UseSharedSearchKeySeed(uniqueSearchKey);
	plg.UseSharedSearchKeySeed(uniqueSearchKey);
	YsHasTextMetaData::UseSharedSearchKeySeed(uniqueSearchKey);

	matrixIsSet=YSFALSE;
	mat.Initialize();
	readSrfVar=NULL;

	searchTable=NULL;

	trustPolygonNormal=YSFALSE;
}

YsShell::~YsShell()
{
	if(NULL!=searchTable)
	{
		DetachSearchTable();
	}

	for(PolygonHandle plHd=nullptr; YSOK==plg.MoveToNextIncludingFrozen(plHd); )
	{
		DetachSpecialAttributeBeforeDeletingPolygon(*plg.GetObjectMaybeFrozen(plHd));
	}
	plg.CleanUp();

	for(VertexHandle vtHd=nullptr; YSOK==vtx.MoveToNextIncludingFrozen(vtHd); )
	{
		DetachSpecialAttributeBeforeDeletingVertex(*vtx.GetObjectMaybeFrozen(vtHd));
	}
	vtx.CleanUp();
}

void YsShell::CleanUp(void)
{
	// 2015/07/26
	//   Noticed that I am not cleaning up frozen vertices and polygons.
	//   Was it my intention?  I don't remember....  I wrote it too long time ago.
	//   But, for backward compatibility, I leave it as is for now.
	for(PolygonHandle plHd=nullptr; YSOK==plg.MoveToNext(plHd); )
	{
		DetachSpecialAttributeBeforeDeletingPolygon(*plg.GetObjectMaybeFrozen(plHd));
	}
	plg.CleanUp();
	for(VertexHandle vtHd=nullptr; YSOK==vtx.MoveToNext(vtHd); )
	{
		DetachSpecialAttributeBeforeDeletingVertex(*vtx.GetObjectMaybeFrozen(vtHd));
	}
	vtx.CleanUp();

	if(searchTable!=NULL)
	{
		auto tablePtr=searchTable;
		DetachSearchTable();
		tablePtr->Prepare();
		AttachSearchTable(tablePtr);
	}
	RewindSearchKey();
	bbxIsValid=YSFALSE;

	vtxMapping.CleanUp();

	YsHasTextMetaData::CleanUp();
}

void YsShell::Encache(void) const
{
	plg.Encache();
	vtx.Encache();
}

void YsShell::Decache(void) const
{
	plg.Decache();
	vtx.Decache();
}


YSRESULT YsShell::AttachSearchTable(YsShellSearchTable *search) const
{
	if(searchTable==NULL)
	{
		if(search!=NULL)
		{
			searchTable=search;

			search->SetShellPointer(this);

			YsShellVertexHandle vtHd;
			YsShellPolygonHandle plHd;

			search->Prepare();
			search->ResizeForShell(*this);
			vtHd=NULL;
			while((vtHd=FindNextVertex(vtHd))!=NULL)
			{
				search->AddVertex(*this,vtHd);
			}
			plHd=NULL;
			while((plHd=FindNextPolygon(plHd))!=NULL)
			{
				search->AddPolygon(*this,plHd);
			}

			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::AttachSearchTable()\n");
		YsErrOut("  Search table is already attached.\n");
		return YSERR;
	}
}

YSRESULT YsShell::DetachSearchTable(void) const
{
	if(searchTable!=NULL)
	{
		searchTable->SetShellPointer(NULL);
		searchTable=NULL;
		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::DetachSearchTable()\n");
		YsErrOut("  Search table is not attached.\n");
		return YSERR;
	}
}

const YsShellSearchTable *YsShell::GetSearchTable(void) const
{
	return searchTable;
}



void YsShell::RewindVtxPtr(void) const
{
	currentVtHd=nullptr;
}

YsShell::VertexHandle YsShell::StepVtxPtr(void) const
{
	currentVtHd=vtx.FindNext(currentVtHd);
	return currentVtHd;
}

void YsShell::RewindPlgPtr(void) const
{
	currentPlHd=nullptr;
}

YsShell::PolygonHandle YsShell::StepPlgPtr(void) const
{
	currentPlHd=plg.FindNext(currentPlHd);
	return currentPlHd;
}



YsShell::VertexHandle YsShell::FindNextVertex(YsShellVertexHandle vtHd) const
{
	return vtx.FindNext(vtHd);
}

YsShell::VertexHandle YsShell::FindPrevVertex(YsShellVertexHandle vtHd) const
{
	return vtx.FindPrev(vtHd);
}

YsShellPolygonHandle YsShell::FindNextPolygon(YsShellPolygonHandle plHd) const
{
	return plg.FindNext(plHd);
}

YsShellPolygonHandle YsShell::FindPrevPolygon(YsShellPolygonHandle plHd) const
{
	return plg.FindPrev(plHd);
}

int YsShell::GetVertexIdFromHandle(VertexHandle vtHd) const
{
	if(vtHd!=NULL)
	{
		return (int)vtx.GetIndexFromHandle(vtHd);
	}
	else
	{
		return -1;
	}
}

int YsShell::GetPolygonIdFromHandle(YsShellPolygonHandle plHd) const
{
	if(plHd!=NULL)
	{
		return (int)plg.GetIndexFromHandle(plHd);
	}
	else
	{
		return -1;
	}
}

YsShell::VertexHandle YsShell::GetVertexHandleFromId(YSSIZE_T vtId) const
{
	return vtx.GetObjectHandleFromIndex(vtId);
}

YsShell::PolygonHandle YsShell::GetPolygonHandleFromId(YSSIZE_T plId) const
{
	return plg.GetObjectHandleFromIndex(plId);
}

YsShellVertexHandle YsShell::AddVertexH(const YsVec3 &vec0)
{
	auto neo=CreateVertex();
	if(neo!=nullptr)
	{
		YsVec3 vec;
		if(matrixIsSet==YSTRUE)
		{
			mat.MulInverse(vec,vec0,1.0);
		}
		else
		{
			vec=vec0;
		}

		vtx[neo]->SetPosition(vec);

		if(searchTable!=NULL)
		{
			searchTable->AddVertex(*this,neo);
		}

		if(bbxIsValid==YSTRUE)
		{
			YsBoundingBoxMaker3 bbx;
			bbx.Begin(bbx1);
			bbx.Add(bbx2);
			bbx.Add(vec);
			bbx.Get(bbx1,bbx2);
		}
		return neo;
	}
	else
	{
		YsErrOut("YsShell::AddVertex\nLowMemoryWarning\n");
		return NULL;
	}
}

YsShellPolygonHandle YsShell::AddPolygonH(YSSIZE_T nv,const int vtId[])
{
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle,16> vtxList;
	YSSIZE_T i;
	vtxList.Set(0,NULL);
	for(i=0; i<nv; i++)
	{
		vtxList.AppendItem(GetVertexHandleFromId(vtId[i]));
	}
	plHd=AddPolygonH(nv,vtxList.GetArray());
	return plHd;
}

YsShell::PolygonHandle YsShell::AddPolygonH(YSSIZE_T nv,const YsShellVertexHandle vtHd[])
{
	PolygonHandle neo=CreatePolygon();
	if(neo!=nullptr)
	{
		YSSIZE_T i;
		plg[neo]->SetVertexList(nv,vtHd);

		for(i=0; i<nv; i++)
		{
			GetVertex(vtHd[i])->IncrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->AddPolygon(*this,neo);
		}
		return neo;
	}
	else
	{
		YsErrOut("YsShell::AddPolygon\nLowMemoryWarning\n");
		return NULL;
	}
}

YsShellPolygonHandle YsShell::AddPolygonVertexH(YSSIZE_T nv,const YsVec3 v[])
{
	return AddPolygonVertex(nv,v,-1,NULL);
}

int YsShell::AddVertex(const YsVec3 &vec)
{
	YsShellVertexHandle vtHd;
	vtHd=AddVertexH(vec);
	return (int)vtx.GetN()-1; // GetVertexIdFromHandle(vtHd);
}

// int YsShell::AddPolygon(int nv,const int v[])  // Deleted on 2005/01/19
// {
// 	YsShellPolygonHandle plHd;
// 	plHd=AddPolygonH(nv,v);
// 	return plg.GetN()-1; // GetPolygonIdFromHandle(plHd);
// }

int YsShell::AddPolygonVertex(YSSIZE_T nv,const YsVec3 v[])
{
	// return GetPolygonIdFromHandle(AddPolygonVertex(nv,v,-1,NULL));

	AddPolygonVertex(nv,v,-1,NULL);
	return (int)plg.GetN()-1;
}

YSRESULT YsShell::ModifyPolygon(int plId,const YSSIZE_T nv,const int v[])
{
	YSSIZE_T i;
	YsShellPolygonHandle plHd;
	YsShellPolygon *plg;
	YsArray <YsShellVertexHandle,16> vtxList;

	plHd=GetPolygonHandleFromId(plId);
	plg=GetPolygon(plHd);
	vtxList.Set(0,NULL);
	for(i=0; i<nv; i++)
	{
		vtxList.AppendItem(GetVertexHandleFromId(v[i]));
	}
	return ModifyPolygon(plHd,nv,vtxList.GetArray());
}

YSRESULT YsShell::ModifyPolygon(YsShellPolygonHandle plHd,YSSIZE_T nv,const YsShellVertexHandle v[])
{
	YsShellPolygon *plg;
	YsShellVertex *vtx;

	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		YSSIZE_T i,nVtxOld;
		nVtxOld=GetNumVertexOfPolygon(plHd);

		for(i=0; i<nVtxOld; i++)
		{
			vtx=GetVertex(plg->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		for(i=0; i<nv; i++)
		{
			vtx=GetVertex(v[i]);
			vtx->IncrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		plg->SetVertexList(nv,v);

		if(searchTable!=NULL)
		{
			searchTable->AddPolygon(*this,plHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::ModifyPolygon(YsShellPolygonHandle plHd,YsConstArrayMask <YsShellVertexHandle> plVtHd)
{
	return ModifyPolygon(plHd,plVtHd.GetN(),plVtHd);
}

YSRESULT YsShell::SetPolygonVertex(PolygonHandle plHd,YSSIZE_T nv,const VertexHandle plVtHd[])
{
	return ModifyPolygon(plHd,nv,plVtHd);
}
YSRESULT YsShell::SetPolygonVertex(PolygonHandle plHd,const YsConstArrayMask <VertexHandle> &plVtHd)
{
	return ModifyPolygon(plHd,plVtHd.GetN(),plVtHd);
}

YSRESULT YsShell::ValidateVtId(YSSIZE_T nVtId,const int vtId[]) const
{
	int i,nVtx;
	nVtx=GetNumVertex();
	for(i=0; i<nVtId; i++)
	{
		if(nVtx<=vtId[i])
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShell::ValidatePlId(YSSIZE_T nPlId,const int plId[]) const
{
	int i,nPlg;
	nPlg=GetNumVertex();
	for(i=0; i<nPlId; i++)
	{
		if(nPlg<=plId[i])
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShell::CheckPotentialPolygon(YSSIZE_T nVt,const YsShellVertexHandle vt[],const double &cosThr)
{
	if(nVt>=3)
	{
		int i;
		YsVec3 p[3],v[2];
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(p[0],vt[i]);
			GetVertexPosition(p[1],vt[(i+1)%nVt]);
			GetVertexPosition(p[2],vt[(i+2)%nVt]);
			v[0]=p[1]-p[0];
			v[1]=p[2]-p[1];
			if(v[0].Normalize()!=YSOK || v[1].Normalize()!=YSOK || v[0]*v[1]>=cosThr)
			{
				return YSERR;
			}
		}

		for(i=0; i<nVt; i++)  // Detect double refering
		{
			int j;
			YsShellVertexHandle ed1[2],ed2[2];
			ed1[0]=vt[i];
			ed1[1]=vt[(i+1)%nVt];
			for(j=i+1; j<nVt; j++)
			{
				ed2[0]=vt[j];
				ed2[1]=vt[(j+1)%nVt];
				if((ed1[0]==ed2[0] && ed1[1]==ed2[1]) ||
				   (ed1[0]==ed2[1] && ed1[1]==ed2[0]))
				{
					return YSERR;
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::ModifyVertexPosition(int vtId,const YsVec3 &neoPos)
{
	return ModifyVertexPosition(GetVertexHandleFromId(vtId),neoPos);
}

YSRESULT YsShell::ModifyVertexPosition(YsShellVertexHandle vtHd,const YsVec3 &neoPos0)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		YsVec3 neoPos;
		if(matrixIsSet==YSTRUE)
		{
			mat.MulInverse(neoPos,neoPos0,1.0);
		}
		else
		{
			neoPos=neoPos0;
		}
		vtx->SetPosition(neoPos);
		bbxIsValid=YSFALSE;
		return YSOK;
	}
	return YSERR;
}

YsShellPolygonHandle YsShell::AddPolygonVertex(YSSIZE_T nv,const YsVec3 v[],int fa3,const int fa4[])
{
	int i;
	YsShellVertexHandle *vtHd;
	YsShellPolygonHandle plHd;
	YsOneMemoryBlock <YsShellVertexHandle,256> memBlock;
	YsShellVertex *vtx;

	vtHd=memBlock.GetMemoryBlock(nv);

	if(vtHd!=NULL)
	{
		for(i=0; i<nv; i++)
		{
			vtHd[i]=AddVertexH(v[i]);
			if(fa3>=0 && fa4!=NULL)
			{
				vtx=GetVertex(vtHd[i]);
				vtx->freeAttribute3=fa3;
				vtx->freeAttribute4=fa4[i];
			}
		}
		plHd=AddPolygonH(nv,vtHd);

		return plHd;
	}
	else
	{
		YsErrOut("YsShell::AddPolygonVertex\nLowMemoryWarning\n");
		return NULL;
	}
}

YSBOOL YsShell::FindRepairFlipDirectionOfOneReliablePolygon
     (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle plHd)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::FindRepairFlipDirectionOfOneReliablePolygon()\n");
		YsErrOut("  This function needs search table.\n");
		return YSFALSE;
	}

	// Avoid overlapping polygons
	// Take advantage of search table!

	YsOneMemoryBlock <YsVec3,128> vtxBlock;
	YsVec3 *vtx,tri[3];
	int nVtxPlg;
	YsVec3 v1,v2,nom,cen;
	YsShellPolygon *plg;

	plg=GetPolygon(plHd);

	if(IsOverlappingPolygon(plHd)==YSTRUE)
	{
		plg->freeAttribute4=1;  // Can do nothing for overlapping polygon
		SetNormalOfPolygon(plHd,YsOrigin());
		return YSFALSE;
	}

	nVtxPlg=GetNumVertexOfPolygon(plHd);
	vtx=vtxBlock.GetMemoryBlock(nVtxPlg);
	GetVertexListOfPolygon(vtx,nVtxPlg,plHd);

	if(YsGetLargestTriangleFromPolygon3(tri,nVtxPlg,vtx)==YSOK)
	{
		v1=tri[1]-tri[0];
		v2=tri[2]-tri[1];
		if(v1.Normalize()==YSOK && v2.Normalize()==YSOK)
		{
			nom=v1^v2;
			cen=(tri[0]+tri[1]+tri[2])/3.0;
			if(nom.Normalize()==YSOK)
			{
				int numIntersect;
				YSSIDE side;
				side=CountRayIntersection(numIntersect,cen,nom,plHd);
				if(side==YSINSIDE)
				{
					nom=-nom;
					InvertPolygon(plHd);
					invertedPlHdList.AppendItem(plHd);
					SetNormalOfPolygon(plHd,nom);
					plg->freeAttribute4=1;
					return YSTRUE;
				}
				else if(side==YSOUTSIDE)
				{
					SetNormalOfPolygon(plHd,nom);
					plg->freeAttribute4=1;
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

YSRESULT YsShell::RecursivelyRepairFlipDirection
    (int &nDone,YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle firstPlHd)
{
	YsArray <YsShellRepairFlipDirectionInfo,4096> *todo;

	todo=new YsArray <YsShellRepairFlipDirectionInfo,4096>;
	if(todo==NULL)
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::RecursivelyRepairFlipDirection()");
		return YSERR;
	}

	AddNeighborToList(*todo,firstPlHd);
	while(todo->GetNumItem()>0)
	{
		YsShellRepairFlipDirectionInfo info;

		YSSIZE_T n=todo->GetNumItem();
		info=todo->GetItem(n-1);

		YsShellPolygon *plg;
		plg=GetPolygon(info.plHd);
		if(plg->freeAttribute4==0)
		{
			nDone++;
			RepairFlipDirectionOfOnePolygon(invertedPlHdList,info);
			plg->freeAttribute4=1;
			AddNeighborToList(*todo,info.plHd);
		}

		todo->Delete(n-1);
	}

	delete todo;
	return YSOK;
}

YSRESULT YsShell::RepairFlipDirectionOfOnePolygon
    (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellRepairFlipDirectionInfo &info)
{
	int i,n;
	YsOneMemoryBlock <YsShellVertexHandle,128> plVtHdBlock;
	YsOneMemoryBlock <YsVec3,128> plVtxBlock;
	YsShellVertexHandle *plVtHd;
	YsVec3 *plVtx,tri[3],nom,v1,v2;

	n=GetNumVertexOfPolygon(info.plHd);
	plVtHd=plVtHdBlock.GetMemoryBlock(n);
	plVtx=plVtxBlock.GetMemoryBlock(n);

	GetVertexListOfPolygon(plVtHd,n,info.plHd);

	for(i=0; i<n; i++)
	{
		if(plVtHd[i]==info.vtHd1 && plVtHd[(i+1)%n]==info.vtHd2)
		{
			break;
		}
		else if(plVtHd[i]==info.vtHd2 && plVtHd[(i+1)%n]==info.vtHd1)
		{
			InvertPolygon(info.plHd);
			invertedPlHdList.AppendItem(info.plHd);
			break; // Is it needed? 2003/07/14
		}
	}

	// At this point, must be already flipped.
	GetVertexListOfPolygon(plVtx,n,info.plHd);
	YsGetLargestTriangleFromPolygon3(tri,n,plVtx);

	v1=tri[1]-tri[0];
	v2=tri[2]-tri[1];

	if(v1.Normalize()==YSOK && v2.Normalize()==YSOK)
	{
		nom=v1^v2;
		if(nom.Normalize()==YSOK)
		{
			SetNormalOfPolygon(info.plHd,nom);
		}
	}

	return YSOK;
}

YSRESULT YsShell::AddNeighborToList
	    (YsArray <YsShellRepairFlipDirectionInfo,4096> &todo,YsShellPolygonHandle plHd)
{
	int i,n;
	YsOneMemoryBlock <YsShellVertexHandle,128> plVtHdBlock;
	YsShellVertexHandle *plVtHd;

	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	YsShellVertexHandle vtHd1,vtHd2;
	YsShellRepairFlipDirectionInfo info;

	n=GetNumVertexOfPolygon(plHd);
	plVtHd=plVtHdBlock.GetMemoryBlock(n);

	GetVertexListOfPolygon(plVtHd,n,plHd);

	for(i=0; i<n; i++)
	{
		vtHd1=plVtHd[i];
		vtHd2=plVtHd[(i+1)%n];

		if(searchTable->FindPolygonListByEdge(nFoundPlHd,foundPlHd,*this,vtHd1,vtHd2)==YSOK && nFoundPlHd==2)
		{
			YsShellPolygonHandle neighborPlHd;
			if(foundPlHd[0]==plHd)
			{
				neighborPlHd=foundPlHd[1];
			}
			else
			{
				neighborPlHd=foundPlHd[0];
			}

			YsShellPolygon *plg;
			plg=GetPolygon(neighborPlHd);
			if(plg->freeAttribute4==0)
			{
				info.plHd=neighborPlHd;
				info.vtHd1=vtHd2;  // Note : Edge direction must be inverted for the neighboring polygon.
				info.vtHd2=vtHd1;  //
				todo.Append(info);
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfPolygon(YsArray <int> &attrib4) const
{
	int i,n;
	YsShellPolygonHandle plHd;

	n=GetNumPolygon();
	if(attrib4.Set(n,NULL)==YSOK)
	{
		plHd=NULL;
		i=0;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			YsShellPolygon *plg;
			plg=(YsShellPolygon *)GetPolygon(plHd);
			attrib4.SetItem(i,plg->freeAttribute4);
			i++;
		}

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShell::RestoreFreeAttribute4OfPolygon(YsArray <int> &attrib4) const
{
	int i;
	YsShellPolygonHandle plHd;

	plHd=NULL;
	i=0;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		YsShellPolygon *plg;
		plg=(YsShellPolygon *)GetPolygon(plHd);
		plg->freeAttribute4=attrib4.GetItem(i);
		i++;
	}

	return YSOK;
}

YSRESULT YsShell::ClearFreeAttribute4OfPolygon(int attrib4)
{
	YsShellPolygonHandle plHd;

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		YsShellPolygon *plg;
		plg=GetPolygon(plHd);
		plg->freeAttribute4=attrib4;
	}

	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfVertex(YsArray <int> &attrib4) const
{
	int i,n;
	YsShellVertexHandle vtHd;

	n=GetNumVertex();
	if(attrib4.Set(n,NULL)==YSOK)
	{
		vtHd=NULL;
		i=0;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsShellVertex *plg;
			plg=(YsShellVertex *)GetVertex(vtHd);
			attrib4.SetItem(i,plg->freeAttribute4);
			i++;
		}

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShell::RestoreFreeAttribute4OfVertex(YsArray <int> &attrib4) const
{
	int i;
	YsShellVertexHandle vtHd;

	vtHd=NULL;
	i=0;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *plg;
		plg=(YsShellVertex *)GetVertex(vtHd);
		plg->freeAttribute4=attrib4.GetItem(i);
		i++;
	}

	return YSOK;
}

YSRESULT YsShell::ClearFreeAttribute4OfVertex(int attrib4)
{
	YsShellVertexHandle vtHd;

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *vtx;
		vtx=GetVertex(vtHd);
		vtx->freeAttribute4=attrib4;
	}

	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfPolygonAndNeighbor(
    YsArray <YsPair <YsShellPolygonHandle,int>,64 > &savePlAtt4,
    int nPl,const YsShellPolygonHandle plHdList[],int freeAtt4Value) const
{
	int i,j,nPlVt;
	const YsShellSearchTable *search;

	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::SaveFreeAttribute4OfPolygonAndNeighbor()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}


	savePlAtt4.Set(0,NULL);

	for(i=0; i<nPl; i++)
	{
		YsPair <YsShellPolygonHandle,int> plAtt4Pair;
		plAtt4Pair.a=plHdList[i];
		plAtt4Pair.b=GetPolygon(plHdList[i])->freeAttribute4;
		GetPolygon(plHdList[i])->freeAttribute4=freeAtt4Value;

		savePlAtt4.Append(plAtt4Pair);

		nPlVt=GetNumVertexOfPolygon(plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			YsShellPolygonHandle neiPlHd;
			neiPlHd=search->GetNeighborPolygon(*this,plHdList[i],j);
			if(neiPlHd!=NULL)
			{
				plAtt4Pair.a=neiPlHd;
				plAtt4Pair.b=GetPolygon(neiPlHd)->freeAttribute4;
				GetPolygon(neiPlHd)->freeAttribute4=freeAtt4Value;

				savePlAtt4.Append(plAtt4Pair);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::RestoreFreeAttribute4OfPolygon(
    int nSavePlAtt4,const YsPair <YsShellPolygonHandle,int> savePlAtt4[]) const
{
	int i;
	for(i=nSavePlAtt4-1; i>=0; i--)
	{
		GetPolygon(savePlAtt4[i].a)->freeAttribute4=savePlAtt4[i].b;
	}
	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfVertexAndNeighbor(
    YsArray <YsPair <YsShellVertexHandle,int>,64> &saveVtAtt4,
    int nVt,const YsShellVertexHandle vtHdList[],int freeAtt4Value) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::SaveFreeAttribute4OfVertexAndNeighbor()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}

	int i,j;
	YsArray <YsShellVertexHandle,16> connVtHd;

	saveVtAtt4.Set(0,NULL);

	for(i=0; i<nVt; i++)
	{
		YsPair <YsShellVertexHandle,int> vtAtt4Pair;
		vtAtt4Pair.a=vtHdList[i];
		vtAtt4Pair.b=GetVertex(vtHdList[i])->freeAttribute4;
		GetVertex(vtHdList[i])->freeAttribute4=freeAtt4Value;

		saveVtAtt4.Append(vtAtt4Pair);

		if(search->GetConnectedVertexList(connVtHd,*this,vtHdList[i])==YSOK)
		{
			forYsArray(j,connVtHd)
			{
				vtAtt4Pair.a=connVtHd[j];
				vtAtt4Pair.b=GetVertex(connVtHd[j])->freeAttribute4;
				GetVertex(connVtHd[j])->freeAttribute4=freeAtt4Value;

				saveVtAtt4.Append(vtAtt4Pair);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::RestoreFreeAttribute4OfVertex(int nSaveVtAtt4,const YsPair <YsShellVertexHandle,int> saveVtAtt4[]) const
{
	int i;
	for(i=nSaveVtAtt4-1; i>=0; i--)
	{
		GetVertex(saveVtAtt4[i].a)->freeAttribute4=saveVtAtt4[i].b;
	}
	return YSOK;
}


YSRESULT YsShell::GetNormalOfPolygon(YsVec3 &nom,int plId) const
{
	return GetNormalOfPolygon(nom,GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::GetColorOfPolygon(YsColor &col,int plId) const
{
	return GetColorOfPolygon(col,GetPolygonHandleFromId(plId));
}

YsVec3 YsShell::GetShellCenter(void) const
{
	YsVec3 sum=YsVec3::Origin();
	if(0<GetNumVertex())
	{
		for(auto vtHd : AllVertex())
		{
			sum+=GetVertexPosition(vtHd);
		}
		sum/=(double)GetNumVertex();
	}
	return sum;
}

const YsVec3 YsShell::GetCenter(YSSIZE_T nVt,const YsShellVertexHandle vtHd[]) const
{
	YsVec3 sum=YsOrigin();
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		sum+=GetVertexPosition(vtHd[idx]);
	}
	if(0<nVt)
	{
		sum/=(double)nVt;
	}
	return sum;
}

const YsVec3 YsShell::GetCenter(YsShellPolygonHandle plHd) const
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	GetPolygon(plVtHd,plHd);
	return GetCenter(plVtHd);
}

const YsVec3 YsShell::GetCenter(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	return (GetVertexPosition(edVtHd0)+GetVertexPosition(edVtHd1))/2.0;
}

// Sorry, my bad.  I removed this function.  Please use GetCenter(Edge edge) instead.
// const YsVec3 YsShell::GetCenter(const YsShellVertexHandle edVtHd[2]) const
// {
// 	return (GetVertexPosition(edVtHd[0])+GetVertexPosition(edVtHd[1]))/2.0;
// }

const YsVec3 YsShell::GetCenter(Edge edge) const
{
	return (GetVertexPosition(edge.edVtHd[0])+GetVertexPosition(edge.edVtHd[1]))/2.0;
}


const YsVec3 &YsShell::GetCenterOfPolygon(YsVec3 &cen,YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	cen=YsOrigin();

	nPlVt=GetNumVertexOfPolygon(plHd);
	plVtHd=GetVertexListOfPolygon(plHd);
	if(nPlVt>0 && plVtHd!=NULL)
	{
		int i;
		YsVec3 vtPos;
		for(i=0; i<nPlVt; i++)
		{
			GetVertexPosition(vtPos,plVtHd[i]);
			cen+=vtPos;
		}
		cen/=double(nPlVt);
	}
	return cen;
}

const YsVec3 &YsShell::GetCenterOfEdge(YsVec3 &mid,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 pos[2];
	if(GetVertexPosition(pos[0],edVtHd1)==YSOK && GetVertexPosition(pos[1],edVtHd2)==YSOK)
	{
		mid=(pos[0]+pos[1])/2.0;
		return mid;
	}
	mid=YsOrigin();
	return mid;
}

const YsVec3 &YsShell::GetCenterOfVertexGroup(YsVec3 &cen,YSSIZE_T  nVt,const YsShellVertexHandle vtHdList[]) const
{
	YSSIZE_T i;
	YsVec3 p;
	cen=YsOrigin();
	if(nVt>0)
	{
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(p,vtHdList[i]);
			cen+=p;
		}
		cen/=(double)nVt;
	}
	return cen;
}

const YsVec3 &YsShell::GetCenterOfTriangle(YsVec3 &cen,YsShellVertexHandle trVtHd1,YsShellVertexHandle trVtHd2,YsShellVertexHandle trVtHd3) const
{
	YsVec3 pos[3];
	if(GetVertexPosition(pos[0],trVtHd1)==YSOK && 
	   GetVertexPosition(pos[1],trVtHd2)==YSOK &&
	   GetVertexPosition(pos[2],trVtHd3)==YSOK)
	{
		cen=(pos[0]+pos[1]+pos[2])/3.0;
		return cen;
	}
	cen=YsOrigin();
	return cen;
}

const YsVec3 &YsShell::GetArbitraryInsidePointOfPolygon(YsVec3 &isp,YsShellPolygonHandle plHd) const
{
	if(GetNumVertexOfPolygon(plHd)==3)
	{
		return GetCenterOfPolygon(isp,plHd);
	}
	else
	{
		YsArray <YsVec3,4> plVtPos;
		GetVertexListOfPolygon(plVtPos,plHd);
		YsGetArbitraryInsidePointOfPolygon3(isp,plVtPos.GetN(),plVtPos);
	}
	return isp;
}

const double YsShell::GetEdgeSquareLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd1)==YSOK &&
	   GetVertexPosition(p2,edVtHd2)==YSOK)
	{
		return (p1-p2).GetSquareLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeSquareLength(YsShellVertexHandle edVtHd[2]) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd[0])==YSOK &&
	   GetVertexPosition(p2,edVtHd[1])==YSOK)
	{
		return (p1-p2).GetSquareLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeSquareLength(Edge edge) const
{
	return GetEdgeSquareLength(edge.edVtHd);
}

double YsShell::GetLength(const YsConstArrayMask <VertexHandle> &vtHdSeq,YSBOOL isLoop) const
{
	double L=0.0;
	if(2<=vtHdSeq.size())
	{
		for(YSSIZE_T idx=0; idx<vtHdSeq.size()-1; ++idx)
		{
			L+=GetEdgeLength(vtHdSeq[idx],vtHdSeq[idx+1]);
		}
		if(YSTRUE==isLoop)
		{
			L+=GetEdgeLength(vtHdSeq.front(),vtHdSeq.back());
		}
	}
	return L;
}

const double YsShell::GetEdgeLength(YsShellPolygonHandle plHd,YSSIZE_T edIdx) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	if(edIdx<nPlVt)
	{
		return GetEdgeLength(plVtHd[edIdx],plVtHd[(edIdx+1)%nPlVt]);
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd1)==YSOK &&
	   GetVertexPosition(p2,edVtHd2)==YSOK)
	{
		return (p1-p2).GetLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(YsShellVertexHandle edVtHd[2]) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd[0])==YSOK &&
	   GetVertexPosition(p2,edVtHd[1])==YSOK)
	{
		return (p1-p2).GetLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(Edge edge) const
{
	return GetEdgeLength(edge.edVtHd);
}

const double YsShell::GetEdgeLength(YSSIZE_T nVt,const YsShellVertexHandle edVtHd[]) const
{
	int i;
	double l;
	YsVec3 buf[2];

	l=0.0;
	if(nVt>=2)
	{
		GetVertexPosition(buf[0],edVtHd[0]);
		for(i=1; i<nVt; i++)
		{
			GetVertexPosition(buf[i&1],edVtHd[i]);
			l+=(buf[0]-buf[1]).GetLength();
		}
	}
	return l;
}

const double YsShell::GetPointToVertexDistance(const YsVec3 &pos,YsShellVertexHandle vtHd) const
{
	return sqrt(GetPointToVertexSquareDistance(pos,vtHd));
}

const double YsShell::GetVertexToPointDistance(YsShellVertexHandle vtHd,const YsVec3 &pos) const
{
	return sqrt(GetPointToVertexSquareDistance(pos,vtHd));
}

const double YsShell::GetPointToVertexSquareDistance(const YsVec3 &pos,YsShellVertexHandle vtHd) const
{
	YsVec3 vtPos;
	GetVertexPosition(vtPos,vtHd);
	return (pos-vtPos).GetSquareLength();
}

const double YsShell::GetVertexToPointSquareDistance(YsShellVertexHandle vtHd,const YsVec3 &pos) const
{
	YsVec3 vtPos;
	GetVertexPosition(vtPos,vtHd);
	return (pos-vtPos).GetSquareLength();
}

const double YsShell::GetLoopLength(YSSIZE_T nVt,const YsShellVertexHandle edVtHd[]) const
{
	int i;
	double l;
	YsVec3 buf[2];

	l=0.0;
	if(nVt>=2)
	{
		GetVertexPosition(buf[1],edVtHd[nVt-1]);
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(buf[i&1],edVtHd[i]);
			l+=(buf[0]-buf[1]).GetLength();
		}
	}
	return l;
}

YSRESULT YsShell::GetArbitraryInsidePointOfPolygonOnPlane(
    YsVec3 &isp,YsShellPolygonHandle plHd,const YsPlane &pln) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	int i,nItsc;
	YsVec3 itsc[2],tst;

	nItsc=0;
	if(GetVertexListOfPolygon(nPlVt,plVtHd,plHd)==YSOK)
	{
		YsShellVertexHandle edVtHd[2];
		YsVec3 edVtPos[2];
		edVtHd[1]=plVtHd[nPlVt-1];
		for(i=0; i<nPlVt; i++)
		{
			edVtHd[0]=edVtHd[1];
			edVtHd[1]=plVtHd[i];
			GetVertexPosition(edVtPos[0],edVtHd[0]);
			GetVertexPosition(edVtPos[1],edVtHd[1]);

			if(pln.GetPenetration(tst,edVtPos[0],edVtPos[1])==YSOK)
			{
				if(nItsc<2)
				{
					itsc[nItsc++]=tst;
				}
				else
				{
					return YSERR;
				}
			}
		}
		if(nItsc==2)
		{
			isp=(itsc[0]+itsc[1])/2.0;
			return YSOK;
		}
	}
	return YSERR;

}

YsVec3 YsShell::GetNormal(YsShellPolygonHandle plHd) const
{
	YsVec3 nom;
	if(YSOK==GetNormal(nom,plHd))
	{
		return nom;
	}
	return YsOrigin();
}

YSRESULT YsShell::GetNormal(YsVec3 &nom,YsShellPolygonHandle plHd) const
{
	return GetNormalOfPolygon(nom,plHd);
}

YsColor YsShell::GetColor(YsShellPolygonHandle plHd) const
{
	YsColor col;
	if(YSOK==GetColorOfPolygon(col,plHd))
	{
		return col;
	}
	return YsWhite();
}

YSRESULT YsShell::GetColor(YsColor &col,YsShellPolygonHandle plHd) const
{
	return GetColorOfPolygon(col,plHd);
}

YSRESULT YsShell::GetNormalOfPolygon(YsVec3 &nom,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		nom=plg->GetNormal();
		if(matrixIsSet==YSTRUE)   // Modified 2000/11/10
		{
			mat.Mul(nom,nom,0.0);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::ComputeNormalOfPolygonFromGeometry(YsVec3 &nom,YsShellPolygonHandle plHd) const
{
	YsArray <YsVec3,4> plVtPos;
	if(GetVertexListOfPolygon(plVtPos,plHd)==YSOK)
	{
		if(plVtPos.GetN()==3)
		{
			nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
			return nom.Normalize();
		}
		else if(YsFindLeastSquareFittingPlaneNormal(nom,plVtPos.GetN(),plVtPos)==YSOK)
		{
			return YSOK;
		}
		else if(YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos)==YSOK)
		{
			return YSOK;
		}
	}

	nom=YsOrigin();
	return YSERR;
}

YSRESULT YsShell::MakePolygonKeyNormalPairFromGeometry(YsPair <unsigned int,YsVec3> &plKeyNomPair,YsShellPolygonHandle plHd) const
{
	plKeyNomPair.a=GetSearchKey(plHd);
	return ComputeNormalOfPolygonFromGeometry(plKeyNomPair.b,plHd);
}

YSRESULT YsShell::MakePolygonKeyNormalPairListFromGeometry(YsArray <YsPair <unsigned int,YsVec3> > &plKeyNomPair,int nPl,const YsShellPolygonHandle plHdList[]) const
{
	int i;
	plKeyNomPair.Set(nPl,NULL);
	for(i=0; i<nPl; i++)
	{
		MakePolygonKeyNormalPairFromGeometry(plKeyNomPair[i],plHdList[i]);
	}
	return YSOK;
}

double YsShell::ComputeDihedralAngle(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2) const
{
	YsVec3 nom[2];
	if(GetNormalOfPolygon(nom[0],plHd1)==YSOK &&
	   GetNormalOfPolygon(nom[1],plHd2)==YSOK)
	{
		const double dotProd=nom[0]*nom[1];
		if(-1.0>=dotProd)
		{
			return YsPi;
		}
		else if(1.0<=dotProd)
		{
			return 0.0;
		}
		return acos(dotProd);
	}
	return 0.0;
}

double YsShell::ComputeEdgeLength(YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const
{
	YsVec3 p[2];
	if(GetVertexPosition(p[0],vtHd1)==YSOK &&
	   GetVertexPosition(p[1],vtHd2)==YSOK)
	{
		return (p[0]-p[1]).GetLength();
	}
	return 0.0;
}

double YsShell::ComputeEdgeLength(int nVt,const YsShellVertexHandle vtHd[]) const
{
	int i;
	YsVec3 p[2];
	double d;

	d=0.0;

	GetVertexPosition(p[1],vtHd[0]);
	for(i=1; i<nVt; i++)
	{
		p[0]=p[1];
		GetVertexPosition(p[1],vtHd[i]);
		d+=(p[0]-p[1]).GetLength();
	}

	return d;
}

double YsShell::ComputeAngleCos(const YsShellVertexHandle vtHd[3]) const
{
	YsVec3 p[3],v[2];
	GetVertexPosition(p[0],vtHd[0]);
	GetVertexPosition(p[1],vtHd[1]);
	GetVertexPosition(p[2],vtHd[2]);

	v[0]=p[1]-p[0];
	v[1]=p[2]-p[1];
	if(v[0].Normalize()==YSOK && v[1].Normalize()==YSOK)
	{
		return v[0]*v[1];
	}
	return 1.0;
}

double YsShell::ComputeMinimumAngleCos(YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	if(GetVertexListOfPolygon(nPlVt,plVtHd,plHd)==YSOK)
	{
		return ComputeMinimumAngleCos(nPlVt,plVtHd);
	}
	else
	{
		return -1.0;
	}
}

double YsShell::ComputeMinimumAngleCos(int nPlVt,const YsShellVertexHandle plVtHd[]) const
{
	if(nPlVt>=3)
	{
		int i;
		double maxCos;
		YsVec3 p1,p2,v1,v2;

		maxCos=-1.0;
		GetVertexPosition(p1,plVtHd[nPlVt-2]);
		GetVertexPosition(p2,plVtHd[nPlVt-1]);
		v2=p2-p1;
		v2.Normalize();
		for(i=0; i<nPlVt; i++)
		{
			p1=p2;
			GetVertexPosition(p2,plVtHd[i]);

			v1=v2;
			v2=p2-p1;
			if(v1.IsNormalized()==YSTRUE && v2.Normalize()==YSOK)
			{
				double c;
				c=-v1*v2;
				if(maxCos<c)
				{
					maxCos=c;
				}
			}
		}
		return maxCos;
	}
	else
	{
		return -1.0;
	}
}

double YsShell::ComputeVolume(void) const
{
	double vol;
	YsShellPolygonHandle plHd;

	vol=0.0;
	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		int i;
		int nPlVtHd;
		const YsShellVertexHandle *plVtHd;
		YsVec3 tri[3];
		double signedVol;

		nPlVtHd=GetNumVertexOfPolygon(plHd);
		plVtHd=GetVertexListOfPolygon(plHd);

		GetVertexPosition(tri[0],plVtHd[0]);
		signedVol=0.0;
		for(i=1; i<nPlVtHd-1; i++)
		{
			GetVertexPosition(tri[1],plVtHd[i]);
			GetVertexPosition(tri[2],plVtHd[i+1]);
			signedVol+=((tri[1]-tri[0])^(tri[2]-tri[0]))*(tri[0])/6.0;
		}
		vol+=signedVol;
	}

	return vol;
}

double YsShell::ComputeTotalArea(void) const
{
	YsShellPolygonHandle plHd=NULL;
	double area=0.0;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		area+=fabs(GetPolygonArea(plHd));
	}
	return area;
}

YSRESULT YsShell::GetColorOfPolygon(YsColor &col,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		col=plg->GetColor();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetNormalOfPolygon(int plId,const YsVec3 &nom)
{
	return SetNormalOfPolygon(GetPolygonHandleFromId(plId),nom);
}

YSRESULT YsShell::SetColorOfPolygon(int plId,const YsColor &col)
{
	return SetColorOfPolygon(GetPolygonHandleFromId(plId),col);
}

YSRESULT YsShell::SetNormalOfPolygon(YsShellPolygonHandle plHd,const YsVec3 &nom)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		if(matrixIsSet==YSTRUE)  // Modified 2000/11/10
		{
			YsVec3 nomTfm;
			mat.MulInverse(nomTfm,nom,0.0);
			plg->SetNormal(nomTfm);
		}
		else
		{
			plg->SetNormal(nom);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetColorOfPolygon(YsShellPolygonHandle plHd,const YsColor &col)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->SetColor(col);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeleteVertex(int vtId)
{
	return DeleteVertex(GetVertexHandleFromId(vtId));
}

YSRESULT YsShell::DeleteVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *v;
	v=GetVertex(vtHd);
	if(v!=NULL)
	{
		if(v->GetReferenceCount()>0)
		{
			YsErrOut("YsShell::DeleteVertex\n  Tried to delete a Vertex in use\n");
			return YSERR;
		}

		if(searchTable!=NULL)
		{
			searchTable->DeleteVertex(*this,vtHd);
		}

		DetachSpecialAttributeBeforeDeletingVertex(*vtx[vtHd]);
		vtx.Delete(vtHd);

		bbxIsValid=YSFALSE;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeletePolygon(int plId)
{
	return DeletePolygon(GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::DeletePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *p;
	YsShellVertex *vtx;
	p=GetPolygon(plHd);

	if(p!=NULL)
	{
		int i,nVtx;

		nVtx=GetNumVertexOfPolygon(plHd);
		for(i=0; i<nVtx; i++)
		{
			vtx=GetVertex(p->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		DetachSpecialAttributeBeforeDeletingPolygon(*plg[plHd]);
		plg.Delete(plHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeleteVertexAtTheSamePosition(int nLatX,int nLatY,int nLatZ)
{
	int nDeleted;
	return DeleteVertexAtTheSamePosition(nDeleted,nLatX,nLatY,nLatZ);
}

YSRESULT YsShell::DeleteVertexAtTheSamePosition(int &nDeleted,int nLatX,int nLatY,int nLatZ)
{
	YsShellVertexHandle vtHdA,vtHdB;
	YsShellVertex *vtxA,*vtxB;
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle> vtxList;
	YsShellLattice ltc(16384);

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		vtxA->SetMapper(*this,vtHdA);
	}

	ltc.SetDomain(*this,nLatX,nLatY,nLatZ);

	int i,j,k;
	const YsShellLatticeElem *elem;
	YsVec3 posA,posB;
	YsLoopCounter ctr;

	ctr.BeginCounter(nLatX*nLatY*nLatZ);
	for(k=0; k<nLatZ; k++)
	{
		for(j=0; j<nLatY; j++)
		{
			for(i=0; i<nLatX; i++)
			{
				ctr.Increment();

				int n,m;
				elem=ltc.GetBlock(i,j,k);
				for(n=0; n<elem->vtxList.GetNumItem(); n++)
				{
					vtHdA=elem->vtxList.GetItem(n);
					GetVertexPosition(posA,vtHdA);
					vtxA=GetVertex(vtHdA);
					for(m=n+1; m<elem->vtxList.GetNumItem(); m++)
					{
						vtHdB=elem->vtxList.GetItem(m);
						vtxB=GetVertex(vtHdB);
						GetVertexPosition(posB,vtHdB);

						if(posA==posB)
						{
							if(vtxA->Mapper(*this)<vtxB->Mapper(*this))
							{
								vtxB->SetMapper(*this,vtxA->Mapper(*this));
							}
							else
							{
								vtxA->SetMapper(*this,vtxB->Mapper(*this));
							}
						}
					}
				}
			}
		}
	}
	ctr.EndCounter();


//	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
//	{
//		vtxA=GetVertex(vtHdA);
//		if(vtxA->mapper==vtHdA)  // means, if not mapped yet
//		{
//			for(vtHdB=FindNextVertex(vtHdA); vtHdB!=NULL; vtHdB=FindNextVertex(vtHdB))
//			{
//				vtxB=GetVertex(vtHdB);
//				if(vtxA->GetPosition()==vtxB->GetPosition())
//				{
//					vtxB->mapper=vtHdA;
//				}
//			}
//		}
//	}

	int nPlg;
	nPlg=GetNumPolygon();
	ctr.BeginCounter(nPlg);
	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		int i;
		YSBOOL needModify;
		YsShellPolygon *plg;
		YsShellVertex *plVt;
		YsShellVertexHandle plVtHd;

		ctr.Increment();

		needModify=YSFALSE;
		plg=GetPolygon(plHd);
		for(i=0; i<plg->GetNumVertex(); i++)
		{
			plVtHd=plg->GetVertexHandle(i);
			plVt=GetVertex(plVtHd);
			if(plVt->Mapper(*this)!=plVtHd)
			{
				needModify=YSTRUE;
				break;
			}
		}

		if(needModify==YSTRUE)
		{
			vtxList.Set(0,NULL);
			for(i=0; i<plg->GetNumVertex(); i++)
			{
				plVtHd=plg->GetVertexHandle(i);
				plVt=GetVertex(plVtHd);

				while(plVtHd!=plVt->Mapper(*this))
				{
					plVtHd=plVt->Mapper(*this);
					plVt=GetVertex(plVtHd);
				}

				vtxList.AppendItem(plVt->Mapper(*this));
			}

			ModifyPolygon(plHd,vtxList.GetNumItem(),vtxList.GetArray());
		}
	}
	ctr.EndCounter();


	YsArray <YsShellVertexHandle> toDel;
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		if(vtxA->Mapper(*this)!=vtHdA)
		{
			toDel.AppendItem(vtHdA);
		}
	}

	nDeleted=0;
	for(i=0; i<toDel.GetNumItem(); i++)
	{
		DeleteVertex(toDel.GetItem(i));
		nDeleted++;
	}

	return YSOK;
}

YSRESULT YsShell::MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(const double &tol)
{
	YsShellVertexHandle vtHdA;
	YsShellVertex *vtxA,*vtxB;

	YsShell3dTree kdTree;
	YsArray <YsShellVertexHandle,128> vtxList;

	int i,nVtx;
	YsVec3 posA,posB,bbx[2];
	YsLoopCounter ctr;

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		vtxA->SetMapper(*this,vtHdA);
	}

	kdTree.PreAllocateNode(GetNumVertex());  // 2004/06/18
	kdTree.Build(*this);

	nVtx=GetNumVertex();
	ctr.BeginCounter(nVtx);
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		ctr.Increment();

		vtxA=GetVertex(vtHdA);
		if(vtxA->Mapper(*this)==vtHdA)
		{
			GetVertexPosition(posA,vtHdA);

			bbx[0]=posA-YsXYZ()*tol;
			bbx[1]=posA+YsXYZ()*tol;

			if(kdTree.MakeVertexList(vtxList,bbx[0],bbx[1])==YSOK)
			{
				if(vtxList.GetN()==0)
				{
					YsErrOut("  KdTree seems to be broken.\n");
				}
				else if(vtxList.GetN()>=2)
				{
					YsShellVertexHandle topMostVtHd;
					topMostVtHd=vtxList.GetTopItem();
					for(i=0; i<vtxList.GetN(); i++)
					{
						if(vtxList[i]<topMostVtHd)
						{
							topMostVtHd=vtxList[i];
						}
					}

					for(i=0; i<vtxList.GetN(); i++)
					{
						vtxB=GetVertex(vtxList[i]);
						vtxB->SetMapper(*this,topMostVtHd);
					}
				}
			}
			else
			{
				YsErrOut("  KdTree Search has been failed.\n");
			}
		}
	}
	ctr.EndCounter();

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		YsShellVertexHandle tracker;
		YsShellVertex *vtx;
		vtx=GetVertex(vtHdA);

		tracker=vtx->Mapper(*this);
		while(tracker!=GetVertex(tracker)->Mapper(*this))
		{
			tracker=GetVertex(tracker)->Mapper(*this);
			if(tracker==vtHdA)
			{
				YsErrOut("Internal Error!\nMapping Loop detected!\n");
				break;
			}
		}
		vtx->SetMapper(*this,tracker);
	}

	return YSOK;
}

YSRESULT YsShell::DeleteVertexAtTheSamePositionByKdTree(int &nDeleted,const double &tol)
{
	YsShellVertexHandle vtHdA;
	YsShellVertex *vtxA;
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle,128> vtxList;
	int i;
	YsLoopCounter ctr;

	MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(tol);

	int nPlg;
	nPlg=GetNumPolygon();
	ctr.BeginCounter(nPlg);
	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		int i;
		YSBOOL needModify;
		YsShellPolygon *plg;
		YsShellVertex *plVt;
		YsShellVertexHandle plVtHd;

		ctr.Increment();

		needModify=YSFALSE;
		plg=GetPolygon(plHd);
		for(i=0; i<plg->GetNumVertex(); i++)
		{
			plVtHd=plg->GetVertexHandle(i);
			plVt=GetVertex(plVtHd);
			if(plVt->Mapper(*this)!=plVtHd)
			{
				needModify=YSTRUE;
				break;
			}
		}

		if(needModify==YSTRUE)
		{
			vtxList.Set(0,NULL);
			for(i=0; i<plg->GetNumVertex(); i++)
			{
				plVtHd=plg->GetVertexHandle(i);
				plVt=GetVertex(plVtHd);

				while(plVtHd!=plVt->Mapper(*this))
				{
					plVtHd=plVt->Mapper(*this);
					plVt=GetVertex(plVtHd);
				}

				vtxList.AppendItem(plVt->Mapper(*this));
			}

			ModifyPolygon(plHd,vtxList.GetNumItem(),vtxList.GetArray());
		}
	}
	ctr.EndCounter();


	YsArray <YsShellVertexHandle> toDel;
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		if(vtxA->Mapper(*this)!=vtHdA)
		{
			toDel.AppendItem(vtHdA);
		}
	}

	nDeleted=0;
	for(i=0; i<toDel.GetNumItem(); i++)
	{
		DeleteVertex(toDel.GetItem(i));
		nDeleted++;
	}

	return YSOK;
}

YSRESULT YsShell::RepairPolygonOrientationBasedOnNormal(void)
{
	YsArray <YsShellPolygonHandle> invertedPlHd;
	return RepairPolygonOrientationBasedOnNormal(invertedPlHd);
}

YSRESULT YsShell::RepairPolygonOrientationBasedOnNormal(YsArray <YsShellPolygonHandle> &invertedPlHdList)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::RepairPolygonOrientationBasedOnNormal()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	YsPrintf("YsShell::RepairPolygonOrientationBasedOnNormal\n");

	invertedPlHdList.Set(0,NULL);

	YsShellPolygonHandle plHd;
	YsArray <int> freeAttrib4Save;
	if(SaveFreeAttribute4OfPolygon(freeAttrib4Save)==YSOK)
	{
		int n;
		YsShellPolygon *plg;
		YsLoopCounter ctr;
		YsArray <YsVec3,4> plVtPos;
		int nDone;

		ClearFreeAttribute4OfPolygon(0);

		n=GetNumPolygon();

		YsPrintf("  Repairing Flip Direction (Pass 1)\n");
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				YsVec3 nom;
				GetNormalOfPolygon(nom,plHd);
				if(nom.GetSquareLength()>YsSqr(YsTolerance))
				{
					YsVec3 tri[3],nomFromFlip;
					GetVertexListOfPolygon(plVtPos,plHd);
					YsGetLargestTriangleFromPolygon3(tri,plVtPos.GetN(),plVtPos);
					nomFromFlip=(tri[1]-tri[0])^(tri[2]-tri[0]);
					if(nom*nomFromFlip<0.0)
					{
						invertedPlHdList.Append(plHd);
						InvertPolygon(plHd);
						plg->freeAttribute4=1;
					}
					RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
				}
			}
		}
		ctr.EndCounter();

		YsPrintf("  Repairing Flip Direction (Pass 2)\n");
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				YsVec3 tri[3],nomFromFlip;
				GetVertexListOfPolygon(plVtPos,plHd);
				YsGetLargestTriangleFromPolygon3(tri,plVtPos.GetN(),plVtPos);
				nomFromFlip=(tri[1]-tri[0])^(tri[2]-tri[0]);
				nomFromFlip.Normalize();
				SetNormalOfPolygon(plHd,nomFromFlip);
				plg->freeAttribute4=1;
				RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
			}
		}
		ctr.EndCounter();

		YsPrintf("  Verifying coverage\n");
		YSRESULT res;
		res=YSOK;
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			YsShellPolygon *plg;
			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				res=YSERR;
			}
		}
		ctr.EndCounter();

		if(res!=YSOK)
		{
			YsErrOut("YsShell::RepairPolygonOrientationBasedOnNormal()\n");
			YsErrOut("  One or more polygon could not be covered.\n");
			YsErrOut("  Maybe because:\n");
			YsErrOut("    (1)Shell is not closed.\n");
			YsErrOut("    (2)Distorted polygon(s) exist\n");
		}

		RestoreFreeAttribute4OfPolygon(freeAttrib4Save);
		return res;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell::RepairPolygonOrientationAssumingClosedShell(void)
{
	YsArray <YsShellPolygonHandle> invertedPlHd;
	return RepairPolygonOrientationAssumingClosedShell(invertedPlHd);
}

YSRESULT YsShell::RepairPolygonOrientationAssumingClosedShell
	    (YsArray <YsShellPolygonHandle> &invertedPlHdList)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::RepairPolygonOrientationAssumingClosedShell()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	YsPrintf("YsShell::RepairFlipDirectionAssumingClosedShell\n");

	invertedPlHdList.Set(0,NULL);

	YsShellPolygonHandle plHd;
	YsArray <int> freeAttrib4Save;
	if(SaveFreeAttribute4OfPolygon(freeAttrib4Save)==YSOK)
	{
		int i,n,nOverlap;
		YsShellPolygon *plg;
		YsLoopCounter ctr;

		ClearFreeAttribute4OfPolygon(0);

		n=GetNumPolygon();

		YsPrintf("  Marking Overlapping Polygon\n");
		plHd=NULL;
		i=0;
		nOverlap=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			if(IsOverlappingPolygon(plHd)==YSTRUE)
			{
				plg=GetPolygon(plHd);
				plg->freeAttribute4=1;
				SetNormalOfPolygon(plHd,YsOrigin());
				nOverlap++;
			}

			i++;
		}
		ctr.EndCounter();
		YsPrintf("[%d overlap]\n",nOverlap);

		YsPrintf("  Repairing Flip Direction\n");
		plHd=NULL;
		i=0;

		int nTst,nDone;
		nTst=0;
		nDone=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				nTst++;
				if(FindRepairFlipDirectionOfOneReliablePolygon(invertedPlHdList,plHd)==YSTRUE)
				{
					nDone++;
					RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
				}
			}

			i++;
		}
		ctr.EndCounter();
		YsPrintf("#TST=%d #DONE=%d\n",nTst,nDone);


		YsPrintf("  Verifying coverage\n");
		YSRESULT res;
		res=YSOK;
		plHd=NULL;
		i=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			YsShellPolygon *plg;
			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				res=YSERR;
			}

			i++;
		}
		ctr.EndCounter();

		if(res!=YSOK)
		{
			YsErrOut("YsShell::RepairFlipDirectionAssumingClosedShell()\n");
			YsErrOut("  One or more polygon could not be covered.\n");
			YsErrOut("  Maybe because:\n");
			YsErrOut("    (1)Shell is not closed.\n");
			YsErrOut("    (2)Distorted polygon(s) exist\n");
		}

		RestoreFreeAttribute4OfPolygon(freeAttrib4Save);
		return res;
	}
	else
	{
		return YSERR;
	}
}

YSBOOL YsShell::IsOverlappingPolygon(YsShellPolygonHandle plHd)  // search table required
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::IsOverlappingPolygon()\n");
		YsErrOut("  Search table required.\n");
		return YSFALSE;
	}

	YsOneMemoryBlock <YsShellVertexHandle,128> vtHdBlock;
	YsShellVertexHandle *vtHd;
	int nVtxPlg;
	YsArray <YsShellPolygonHandle,16> plgList;

	nVtxPlg=GetNumVertexOfPolygon(plHd);

	vtHd=vtHdBlock.GetMemoryBlock(nVtxPlg);

	GetVertexListOfPolygon(vtHd,nVtxPlg,plHd);

	searchTable->FindPolygonListByVertexList(plgList,*this,nVtxPlg,vtHd);
	if(plgList.GetN()>=2)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

const YsShell &YsShell::operator=(const YsShell &incoming)
{
	CopyFrom(incoming);
	return *this;
}

const YsShell &YsShell::CopyFrom(const YsShell &from)
{
	if(this!=&from)
	{
		from.Encache();

		CleanUp();

		YsShellVertexHandle fromVtHd;
		YsShellPolygonHandle fromPlHd;

		for(fromVtHd=from.FindNextVertex(NULL); fromVtHd!=NULL; fromVtHd=from.FindNextVertex(fromVtHd))
		{
			YsShellVertex *neoVtx;
			YsShellVertexHandle neoVtHd;
			const YsShellVertex *fromVtx;

			fromVtx=from.GetVertex(fromVtHd);
			neoVtHd=AddVertexH(fromVtx->GetPosition());
			neoVtx=GetVertex(neoVtHd);

			neoVtx->CopyAttribute(*fromVtx);
		}

		Encache();

		YsArray <int> vtIdList;
		for(fromPlHd=from.FindNextPolygon(NULL); fromPlHd!=NULL; fromPlHd=from.FindNextPolygon(fromPlHd))
		{
			YsShellPolygon *neoPlg;
			YsShellPolygonHandle neoPlHd;
			const YsShellPolygon *fromPlg;


			fromPlg=from.GetPolygon(fromPlHd);

			vtIdList.Set(fromPlg->GetNumVertex(),NULL);
			from.GetVertexListOfPolygon((int *)vtIdList,vtIdList.GetN(),from.GetPolygonIdFromHandle(fromPlHd));
			neoPlHd=AddPolygonH(vtIdList.GetN(),(int *)vtIdList);
			neoPlg=GetPolygon(neoPlHd);

			neoPlg->SetNormal(fromPlg->GetNormal());  // Added 2000/11/10
			neoPlg->SetColor(fromPlg->GetColor());    // Added 2000/11/10

			neoPlg->CopyAttribute(*fromPlg);
		}

		YsHasTextMetaData::CopyFrom(from);

		mat=from.mat;
		matrixIsSet=from.matrixIsSet;
		trustPolygonNormal=from.trustPolygonNormal;

		readSrfVar=NULL;
	}

	return *this;
}

int YsShell::GetNumPolygon(void) const
{
	return (int)plg.GetN();
}

YsShellPolygon *YsShell::GetPolygon(int id)
{
	auto plHd=GetPolygonHandleFromId(id);
	return GetPolygon(plHd);
}

const YsShellPolygon *YsShell::GetPolygon(int id) const
{
	auto plHd=GetPolygonHandleFromId(id);
	return GetPolygon(plHd);
}

YSBOOL YsShell::IsFrozen(YsShellPolygonHandle plHd) const
{
	return plg.IsFrozen(plHd);
}

YSBOOL YsShell::IsFrozen(YsShellVertexHandle vtHd) const
{
	return vtx.IsFrozen(vtHd);
}

void YsShell::GetBoundingBox(YsVec3 &min,YsVec3 &max) const
{
	if(bbxIsValid==YSTRUE)
	{
		min=bbx1;
		max=bbx2;
	}
	else
	{
		YsShellVertexHandle vtHd;
		YsBoundingBoxMaker3 bbx;

		vtHd=FindNextVertex(NULL);
		if(vtHd!=NULL)
		{
			YsVec3 vtxPos;

			GetVertexPosition(vtxPos,vtHd);
			bbx.Begin(vtxPos);
			while(vtHd!=NULL)
			{
				GetVertexPosition(vtxPos,vtHd);
				bbx.Add(vtxPos);
				vtHd=FindNextVertex(vtHd);
			}
			bbx.Get(min,max);

			bbx1=min;
			bbx2=max;
			bbxIsValid=YSTRUE;
		}
		else
		{
			min.Set(0.0,0.0,0.0);
			max.Set(0.0,0.0,0.0);
			bbxIsValid=YSFALSE;  // <- May not necessary.  Defensive coding.
		}
	}
}

void YsShell::GetBoundingBox(YsVec3 bbx[2]) const
{
	return GetBoundingBox(bbx[0],bbx[1]);
}

void YsShell::GetBoundingBox(YsVec3 &min,YsVec3 &max,YSSIZE_T nVtx,const VertexHandle vtHd[]) const
{
	YsBoundingBoxMaker3 mkBbx;
	for(YSSIZE_T i=0; i<nVtx; ++i)
	{
		mkBbx.Add(GetVertexPosition(vtHd[i]));
	}
	mkBbx.Get(min,max);
}
void YsShell::GetBoundingBox(YsVec3 minmax[2],YSSIZE_T nVtx,const VertexHandle vtHd[]) const
{
	GetBoundingBox(minmax[0],minmax[1],nVtx,vtHd);
}

void YsShell::GetBoundingBox(YsVec3 &min,YsVec3 &max,YsConstArrayMask <VertexHandle> vtHd) const
{
	GetBoundingBox(min,max,vtHd.GetN(),vtHd);
}

void YsShell::GetBoundingBox(YsVec3 minmax[2],YsConstArrayMask <VertexHandle> vtHd) const
{
	GetBoundingBox(minmax[0],minmax[1],vtHd);
}

YsVec3 YsShell::GetBoundingBoxCenter(void) const
{
	YsVec3 min,max,cen;
	GetBoundingBox(min,max);
	cen=(min+max)/2.0;
	return cen;
}

double YsShell::GetBoundingBoxDiagonalLength(void) const
{
	YsVec3 bbx[2];
	GetBoundingBox(bbx);
	return (bbx[1]-bbx[0]).GetLength();
}

int YsShell::GetNumVertex(void) const
{
	return (int)vtx.GetN();
}

int YsShell::GetMaxNumVertexOfPolygon(void) const
{
	int n,t;
	int i;
	n=0;
	for(i=0; i<GetNumPolygon(); i++)
	{
		t=GetNumVertexOfPolygon(i);
		n=YsGreater(n,t);
	}
	return n;
}

YSRESULT YsShell::GetVertexPosition(YsVec3 &pos,int vtId) const
{
	return GetVertexPosition(pos,GetVertexHandleFromId(vtId));
}

YsArray <YsVec3> YsShell::GetMultiVertexPosition(const YsConstArrayMask <VertexHandle> &vtHdArray) const
{
	YsArray <YsVec3> vtPos;
	vtPos.Resize(vtHdArray.GetN());
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN(); ++idx)
	{
		vtPos[idx]=GetVertexPosition(vtHdArray[idx]);
	}
	return vtPos;
}

int YsShell::GetVertexReferenceCount(int vtId) const
{
	return GetVertexReferenceCount(GetVertexHandleFromId(vtId));
}

// 10/22/2001
YSRESULT YsShell::GetAverageNormalAtVertex(YsVec3 &nom,YsShellVertexHandle vtHd) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::GetAverageNormalAtVertex()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	if(search->FindPolygonListByVertex(nFoundPlHd,foundPlHd,*this,vtHd)==YSOK && nFoundPlHd>0)
	{
		int i;
		YsVec3 sub;
		nom=YsOrigin();
		for(i=0; i<nFoundPlHd; i++)
		{
			GetNormalOfPolygon(sub,foundPlHd[i]);
			nom+=sub;
		}
		nom.Normalize();
		return YSOK;
	}
	else
	{
		nom=YsOrigin();
		return YSOK;
	}
}

YSHASHKEY YsShell::GetSearchKey(VertexHandle vtHd) const
{
	return vtx.GetSearchKey(vtHd);
}

YSHASHKEY YsShell::GetSearchKey(YsShellPolygonHandle plHd) const
{
	return plg.GetSearchKey(plHd);
}

YSHASHKEY YsShell::GetSearchKeyMaybeFrozen(YsShellVertexHandle vtHd) const
{
	return vtx.GetSearchKeyMaybeFrozen(vtHd);
}

YSHASHKEY YsShell::GetSearchKeyMaybeFrozen(YsShellPolygonHandle plHd) const
{
	return plg.GetSearchKeyMaybeFrozen(plHd);
}

int YsShell::GetVertexReferenceCount(YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetReferenceCount();
	}
	else
	{
		return -1;
	}
}

YsShellVertex *YsShell::GetVertex(int id)
{
	YsShellVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(id);
	return GetVertex(vtHd);
}

const YsShellVertex *YsShell::GetVertex(int id) const
{
	YsShellVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(id);
	return GetVertex(vtHd);
}

const int YsShell::GetPolygonNumVertex(YsShellPolygonHandle plHd) const
{
	if(plHd!=NULL)
	{
		return GetPolygon(plHd)->GetNumVertex();
	}
	else
	{
		return 0;
	}
}

int YsShell::GetNumVertexOfPolygon(int plId) const
{
	return GetNumVertexOfPolygon(GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::GetPolygon(int &nPlVt,const YsShellVertexHandle *&plVtHd,YsShellPolygonHandle plHd) const
{
	return GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
}

YSRESULT YsShell::GetPolygonMaybeFrozen(int &nPlVt,const YsShellVertexHandle *&plVtHd,YsShellPolygonHandle plHd) const
{
	if(NULL!=plHd)
	{
		auto plgPtr=plg.GetObjectMaybeFrozen(plHd);
		nPlVt=plgPtr->GetNumVertex();
		plVtHd=plgPtr->GetVertexArray();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(int &nPlVt,const YsShellVertexHandle *&plVtHd,YsShellPolygonHandle plHd) const
{
	nPlVt=GetNumVertexOfPolygon(plHd);
	plVtHd=GetVertexListOfPolygon(plHd);
	if(nPlVt>0 && plVtHd!=NULL)
	{
		return YSOK;
	}
	return YSERR;
}

int YsShell::GetNumVertexOfPolygon(YsShellPolygonHandle plHd) const
{
	return GetPolygonNumVertex(plHd);
}

YSRESULT YsShell::GetVertexListOfPolygon(int idx[],YSSIZE_T maxcount,int plId) const
{
	const YsShellPolygon *plg=GetPolygon(plId);
	if(plg!=NULL)
	{
		YSSIZE_T i,nVtId;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			idx[i]=plg->GetVertexId(*this,i);
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsVec3 v[],YSSIZE_T maxcount,int plId) const
{
	YsShellPolygonHandle plHd;
	plHd=GetPolygonHandleFromId(plId);
	return GetVertexListOfPolygon(v,maxcount,plHd);
}

// Discarded on 2001/04/26
//const int *YsShell::GetVertexListOfPolygon(int plId) const
//{
//	const YsShellPolygon *plg;
//	plg=GetPolygon(plId);
//	if(plg!=NULL)
//	{
//		return plg->GetIndexArray();
//	}
//	return NULL;
//}

YSRESULT YsShell::GetVertexListOfPolygon(YsShellVertexHandle vtHd[],YSSIZE_T maxcount,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		int i,nVtId;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			vtHd[i]=plg->GetVertexHandle(i);
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsVec3 v[],YSSIZE_T maxcount,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		int i,nVtId;
		YsShellVertexHandle vtHd;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			vtHd=plg->GetVertexHandle(i);

			const YsShellVertex *vtx;
			vtx=GetVertex(vtHd);
			if(vtx!=NULL)
			{
				if(matrixIsSet==YSTRUE)
				{
					v[i]=mat*vtx->GetPosition();
				}
				else
				{
					v[i]=vtx->GetPosition();
				}
			}
			else
			{
				return YSERR;
			}
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

const YsShellVertexHandle *YsShell::GetVertexListOfPolygon(YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetVertexArray();
	}
	return NULL;
}

const double YsShell::GetPolygonArea(YsShellPolygonHandle plHd) const
{
	int i;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsVec3 totalCrsPrd,crsPrd,o,p1,p2;

	nPlVt=GetNumVertexOfPolygon(plHd);
	if(nPlVt>=3)
	{
		plVtHd=GetVertexListOfPolygon(plHd);
		totalCrsPrd=YsOrigin();
		GetVertexPosition(o,plVtHd[0]);
		GetVertexPosition(p2,plVtHd[1]);
		for(i=1; i<nPlVt-1; i++)
		{
			p1=p2;
			GetVertexPosition(p2,plVtHd[i+1]);
			crsPrd=(p2-o)^(p1-o);
			totalCrsPrd+=crsPrd;
		}
		return totalCrsPrd.GetLength()/2.0;
	}
	return 0.0;
}

double YsShell::GetAreaOfPolygon(YsShellPolygonHandle plHd) const
{
	return GetPolygonArea(plHd);
}

double YsShell::GetPolygonEdgeLength(YsShellPolygonHandle plHd,int edId) const
{
	YsShellVertexHandle edVtHd[2];
	if(GetPolygonEdgeVertex(edVtHd,plHd,edId)==YSOK)
	{
		return GetEdgeLength(edVtHd[0],edVtHd[1]);
	}
	return 0.0;
}

YsShell::Edge YsShell::GetPolygonEdge(YsShellPolygonHandle plHd,YSSIZE_T edIdx) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

	Edge edge;
	edge.edVtHd[0]=plVtHd[edIdx];
	edge.edVtHd[1]=plVtHd[(edIdx+1)%nPlVt];

	return edge;
}

int YsShell::GetPolygonEdgeIndex(YsShellPolygonHandle plHd,const Edge &edge) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetPolygon(nPlVt,plVtHd,plHd);
	return GetPolygonEdgeIndex(nPlVt,plVtHd,edge[0],edge[1]);
}
int YsShell::GetPolygonEdgeIndex(YsShellPolygonHandle plHd,const YsShellVertexHandle edVtHd[2]) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetPolygon(nPlVt,plVtHd,plHd);
	return GetPolygonEdgeIndex(nPlVt,plVtHd,edVtHd[0],edVtHd[1]);
}
int YsShell::GetPolygonEdgeIndex(YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetPolygon(nPlVt,plVtHd,plHd);
	return GetPolygonEdgeIndex(nPlVt,plVtHd,edVtHd0,edVtHd1);
}
int YsShell::GetPolygonEdgeIndex(YSSIZE_T nPlVt,const YsShellVertexHandle plVtHd[],YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YsConstArrayMask <YsShellVertexHandle> plg(nPlVt,plVtHd);
	for(int idx=0; idx<nPlVt; ++idx)
	{
		if(YSTRUE==YsSameEdge(edVtHd0,edVtHd1,plg[idx],plg.GetCyclic(idx+1)))
		{
			return idx;
		}
	}
	return -1;
}

YSBOOL YsShell::IsPolygonUsingVertex(YsShellPolygonHandle plHd,YsShellVertexHandle vtHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	return YsIsIncluded(nPlVt,plVtHd,vtHd);
}

YSBOOL YsShell::IsPolygonUsingEdgePiece(PolygonHandle plHd,const VertexHandle edVtHd[2]) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetPolygon(nPlVt,plVtHd,plHd);
	for(int i=0; i<nPlVt; ++i)
	{
		if(YSTRUE==YsSameEdge(edVtHd[0],edVtHd[1],plVtHd[i],plVtHd[(i+1)%nPlVt]))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

double YsShell::GetHeightOfPolygon(YsShellPolygonHandle plHd,YsShellVertexHandle baseEdVtHd[2]) const
{
	int i,nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsVec3 basePos[2],tst;
	double hei,d;

	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	GetVertexPosition(basePos[0],baseEdVtHd[0]);
	GetVertexPosition(basePos[1],baseEdVtHd[1]);

	hei=0;
	for(i=0; i<nPlVt; i++)
	{
		GetVertexPosition(tst,plVtHd[i]);
		d=YsGetPointLineDistance3(basePos[0],basePos[1],tst);
		if(d>hei)
		{
			hei=d;
		}
	}
	return hei;
}

YSRESULT YsShell::GetPolygonGroupByNormalSimilarity
   (YsArray <YsShellPolygonHandle,16> &plHdList,YsShellPolygonHandle seedPlHd,
    YsKeyStore *excludePlKey,const double &cosThr) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search!=NULL)
	{
		YsVec3 refNom;
		YsKeyStore visited(256);
		YsArray <YsShellPolygonHandle,16> todo;

		visited.EnableAutoResizing(256,GetNumPolygon()/4+256);

		GetNormalOfPolygon(refNom,seedPlHd);

		todo.Append(seedPlHd);
		plHdList.Set(seedPlHd);
		visited.AddKey(GetSearchKey(seedPlHd));
		while(todo.GetN()>0)
		{
			int i,nPlVt;
			YsShellPolygonHandle plHd,neiPlHd;
			YsVec3 neiNom;

			plHd=todo[0];
			nPlVt=GetNumVertexOfPolygon(plHd);
			for(i=0; i<nPlVt; i++)
			{
				neiPlHd=search->GetNeighborPolygon(*this,plHd,i);
				if(neiPlHd!=NULL && visited.IsIncluded(GetSearchKey(neiPlHd))!=YSTRUE)
				{
					visited.AddKey(GetSearchKey(neiPlHd));

					if(excludePlKey!=NULL && excludePlKey->IsIncluded(GetSearchKey(neiPlHd))==YSTRUE)
					{
						continue;
					}

					GetNormalOfPolygon(neiNom,neiPlHd);
					if(neiNom*refNom>=cosThr)
					{
						todo.Append(neiPlHd);
						plHdList.Append(neiPlHd);
					}
				}
			}

			todo.DeleteBySwapping(0);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::GetPolygonGroupByNormalSimilarity()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}
}

YSRESULT YsShell::GetCircumferentialAngleOfPolygonGroup(
    double &minAngle,YsShellVertexHandle &minVtHd,double &maxAngle,YsShellVertexHandle &maxVtHd,
    const YsVec3 &org,const YsVec3 &axs,int nPl,const YsShellPolygonHandle *plHdList) const
{
	if(nPl==0)
	{
		minAngle=0.0;
		minVtHd=NULL;
		maxAngle=0.0;
		maxVtHd=NULL;
		return YSERR;
	}

	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search!=NULL)
	{
		int i,j,nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsKeyStore visitedVtx(nPl+1),usedVtx(nPl+1);
		YsArray <YsPair <YsShellVertexHandle,double>,16> todo;
		YsPair <YsShellVertexHandle,double> vtxAnglePair;
		YsArray <YsShellVertexHandle,16> connVtHd;

		for(i=0; i<nPl; i++)
		{
			GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
			for(j=0; j<nPlVt; j++)
			{
				usedVtx.AddKey(GetSearchKey(plVtHd[j]));
			}
		}

		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[0]);
		vtxAnglePair.a=plVtHd[0];
		vtxAnglePair.b=0.0;
		visitedVtx.AddKey(GetSearchKey(plVtHd[0]));

		todo.Append(vtxAnglePair);
		minAngle=0.0;
		minVtHd=plVtHd[0];
		maxAngle=0.0;
		maxVtHd=plVtHd[0];

		while(todo.GetN()>0)
		{
			YsVec3 v1,v2,root;
			search->GetConnectedVertexList(connVtHd,*this,todo[0].a);

			GetVertexPosition(v1,todo[0].a);
			root=org+axs*((v1-org)*axs);
			v1-=root;
			if(v1.Normalize()==YSOK)
			{
				forYsArray(i,connVtHd)
				{
					if(usedVtx.IsIncluded(GetSearchKey(connVtHd[i]))==YSTRUE &&
					   visitedVtx.IsIncluded(GetSearchKey(connVtHd[i]))!=YSTRUE)
					{
						double d,a;

						visitedVtx.AddKey(GetSearchKey(connVtHd[i]));

						GetVertexPosition(v2,connVtHd[i]);
						root=org+axs*((v2-org)*axs);
						v2-=root;

						if(v2.Normalize()==YSOK)
						{
							d=acos(v1*v2);
							if((v1^v2)*axs>=0.0)
							{
								a=todo[0].b+d;
							}
							else
							{
								a=todo[0].b-d;
							}

							if(maxAngle<a)
							{
								maxAngle=a;
								maxVtHd=connVtHd[i];
							}
							if(a<minAngle)
							{
								minAngle=a;
								minVtHd=connVtHd[i];
							}

							vtxAnglePair.a=connVtHd[i];
							vtxAnglePair.b=a;
							todo.Append(vtxAnglePair);
						}
					}
				}
			}

			todo.DeleteBySwapping(0);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::GetCircumferentialAngleOfPolygonGroup()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}
}

void YsShell::GetMatrix(YsMatrix4x4 &m) const
{
	m=mat;
}

YSSIDE YsShell::CountRayIntersection
    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,int plIdSkip,YSBOOL watch) const
{
	YsShellPolygonHandle plHd;

	if(watch==YSTRUE)
	{
		YsPrintf("YsShell::CountRayIntersection <- plIdSkip\n");
	}

	if(plIdSkip>=0)
	{
		plHd=GetPolygonHandleFromId(plIdSkip);
	}
	else
	{
		plHd=NULL;
	}
	return CountRayIntersection(CrossCounter,org,vec,plHd,watch);
}

YSSIDE YsShell::CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,YsShellPolygonHandle plHdSkip,YSBOOL watch) const
{
	YsShellPolygonStore excludePlHd(*this);
	excludePlHd.AddPolygon(plHdSkip);
	return CountRayIntersection(CrossCounter,org,vec,NULL,&excludePlHd,watch);
}

YSSIDE YsShell::CountRayIntersection
    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,const YsShellPolygonStore *limitToThesePlg,const YsShellPolygonStore *excludePlg,YSBOOL watch) const
{
	YsShellPolygonHandle plHd;
	int nVtx;
	YsVec3 crs;
	YsArray <YsVec3> vtx;
	YsPlane pln;

	CrossCounter=0;

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if(NULL!=limitToThesePlg && YSTRUE!=limitToThesePlg->IsIncluded(plHd))
		{
			continue;
		}
		if(NULL!=excludePlg && YSTRUE==excludePlg->IsIncluded(plHd))
		{
			continue;
		}

		nVtx=GetNumVertexOfPolygon(plHd);
		GetVertexListOfPolygon(vtx,plHd);

		YsVec3 nom;
		if(TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(nom,plHd)==YSOK && nom!=YsOrigin())
		{
			pln.Set(vtx[0],nom);
		}
		else if(pln.MakeBestFitPlane(nVtx,vtx)!=YSOK)
		{
			if(YSTRUE==watch)
			{
				YsErrOut("YsShell::CountRayIntersection\n  Skipping Bad polygon\n");
			}
			// int j;
			// for(j=0; j<nVtx; j++)
			// {
			// 	YsErrOut("(%lf %lf %lf)\n",vtx[j].x(),vtx[j].y(),vtx[j].z());
			// }
			continue;
		}



		if(pln.GetIntersection(crs,org,vec)==YSOK)
		{
			if(crs==org)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(crs,nVtx,vtx,pln.GetNormal());
				if(side==YSBOUNDARY || side==YSINSIDE)
				{
					return YSBOUNDARY;
				}
			}
			else if(vec*(crs-org)>=0.0)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(crs,nVtx,vtx,pln.GetNormal());
				if(side==YSBOUNDARY || side==YSUNKNOWNSIDE)
				{
					return YSUNKNOWNSIDE;
				}
				if(side==YSINSIDE)
				{
					if(watch==YSTRUE)
					{
						YsPrintf("Intersect Key=%d,Id=%d,(%s)\n",
						    GetSearchKey(plHd),GetPolygonIdFromHandle(plHd),crs.Txt());
						int i;
						YsPrintf("nVtx=%d\n",nVtx);
						for(i=0; i<nVtx; i++)
						{
							YsPrintf("(%s)\n",vtx[i].Txt());
						}
					}
					CrossCounter++;
				}
			}
		}
		else if(pln.CheckOnPlane(org)==YSTRUE)
		{
			YSSIDE side;
			side=YsCheckInsidePolygon3(org,nVtx,vtx,pln.GetNormal());
			if(side==YSBOUNDARY || side==YSINSIDE)
			{
				return YSBOUNDARY;
			}
		}
	}

	if(watch==YSTRUE)
	{
		YsPrintf("Counter=%d\n",CrossCounter);
	}

	if(CrossCounter%2==0)
	{
		return YSOUTSIDE;
	}
	else
	{
		return YSINSIDE;
	}
}

YSSIDE YsShell::CheckInsideSolid(const YsVec3 &pnt,const YsShellPolygonStore *limitToThesePlg,const YsShellPolygonStore *excludePlg,YSBOOL watch) const
{
	int c;
	double h,p;
	YsVec3 vec;
	int CrossCounter;

	YSSIDE side;

	h=YsDegToRad(0);
	p=YsDegToRad(-90);
	for(c=0; c<256; c++)
	{
		vec.Set(cos(p)*cos(h),sin(p),cos(p)*sin(h));

		if(watch==YSTRUE)
		{
			YsPrintf("(%s)-(%s) %s\n",pnt.Txt(),(pnt+vec).Txt(),vec.Txt());
		}

		side=CountRayIntersection(CrossCounter,pnt,vec,limitToThesePlg,excludePlg,watch);

		if(side!=YSUNKNOWNSIDE)
		{
			return side;
		}

		p+=YsDegToRad(30.0);
		h+=YsDegToRad(95.0);
	}

	YsErrOut("YsShell::IsInsideSolid\n  Could not determine inside/outside/boundary.\n");
	return YSUNKNOWNSIDE;
}

YSRESULT YsShell::InvertPolygon(int plId)
{
	YsShellPolygonHandle plHd;
	plHd=GetPolygonHandleFromId(plId);
	return InvertPolygon(plHd);
}

YSRESULT YsShell::InvertPolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->Invert();
		return YSOK;
	}
	return YSERR;
}


YSRESULT YsShell::ShootFiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &p1,const YsVec3 &p2,const YsShellLattice *ltc) const
{
	int i,nVtx;
	YsVec3 vtx[256];
	YsArray <YsShellPolygonHandle,16> plgList;
	YsShellPolygonHandle plHd;

	is.Set(0,NULL);
	plHdLst.Set(0,NULL);

	if(ltc!=NULL)
	{
		YsVec3 r[2];
		r[0]=p1;
		r[1]=p2;
		ltc->MakePossibllyCollidingPolygonList(plgList,2,r);
	}
	else
	{
		plgList.Set(GetNumPolygon(),NULL);
		plHd=NULL;
		i=0;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			plgList.SetItem(i,plHd);
			i++;
		}
	}

	for(i=0; i<plgList.GetNumItem(); i++)
	{
		plHd=plgList[i];
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,itsc;
			YsPlane pln;

			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK && n!=YsOrigin()) ||
			   YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(itsc,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(itsc,nVtx,vtx);
					if(side==YSINSIDE || side==YSBOUNDARY)
					{
						is.AppendItem(itsc);
						plHdLst.AppendItem(plHd);
					}
				}
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::ShootInfiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &org,const YsVec3 &vec,const YsShellLattice *ltc) const
{
	is.Set(0,NULL);
	plHdLst.Set(0,NULL);

	YsVec3 bbx1,bbx2;
	GetBoundingBox(bbx1,bbx2);

	// 2011/12/31 To deal with when one of the dimensions of the bounding box is zero.
	const YsVec3 diagonal=bbx2-bbx1;
	const double margin=diagonal.GetLongestDimension()/50.0;
	bbx1.SubX(margin);
	bbx1.SubY(margin);
	bbx1.SubZ(margin);
	bbx2.AddX(margin);
	bbx2.AddY(margin);
	bbx2.AddZ(margin);

	// The following margine didn't work when one of the dimensions of the bounding box is zero. 
	// const YsVec3 cen=(bbx1+bbx2)/2.0;
	// bbx1=cen+(bbx1-cen)*1.02;  // 2% Safety Margine  <- Actually, needed for cubes.
	// bbx2=cen+(bbx2-cen)*1.02;  //                       in order to make GetPenetration return YSOK

	YsVec3 clip1,clip2;
	if(YsClipInfiniteLine3(clip1,clip2,org,vec,bbx1,bbx2)==YSOK)
	{
		double ip1,ip2;
		ip1=(clip1-org)*vec;
		ip2=(clip2-org)*vec;

		if(ip1>=0.0 && ip2>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,clip1,clip2,ltc);
		}
		else if(ip1>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,org,clip1,ltc);
		}
		else if(ip2>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,org,clip2,ltc);
		}
	}

	// Falling into this point means either:
	//   1. even the ray didn't hit the bounding box, or
	//   2. the ray did hit the bounding box, but both intersections were behind.

	return YSOK;
}

int YsShell::ShootRay(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	YsShellPolygonHandle plHd;
	plHd=ShootRayH(intersect,p1,vec);
	return GetPolygonIdFromHandle(plHd);
}

YsShellPolygonHandle YsShell::ShootRayH(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	int nVtx;
	YsVec3 vtx[256],p2;

	p2=p1+vec;

	YsShellPolygonHandle plHd;
	YsShellPolygonHandle possiblePlHd;
	double possibleDistance;
	YsVec3 possibleCollidePoint;

	possiblePlHd=NULL;
	possibleDistance=(p2-p1).GetSquareLength();
	possibleCollidePoint.Set(0.0,0.0,0.0);

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,i;
			YsPlane pln;

			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK && n!=YsOrigin()) ||
			   YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(i,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(i,nVtx,vtx);
					if((side==YSINSIDE || side==YSBOUNDARY) &&
					   (p1-i).GetSquareLength()<possibleDistance)
					{
						possibleDistance=(p1-i).GetSquareLength();
						possibleCollidePoint=i;
						possiblePlHd=plHd;
					}
				}
			}
		}
	}

	if(possiblePlHd!=NULL)
	{
		intersect=possibleCollidePoint;
	}

	return possiblePlHd;
}

YsShell::PolygonAndPos YsShell::FindFirstIntersection(const YsVec3 &from,const YsVec3 dir) const
{
	PolygonAndPos res;
	res.plHd=nullptr;
	res.pos=from;

	YsShellPolygonHandle candidatePlHd=nullptr;
	double candidateDistance=0.0;
	YsVec3 candidatePoint=YsVec3::Origin();

	for(auto plHd : AllPolygon())
	{
		auto plVtPos=GetPolygonVertexPosition(plHd);
		if(3<=plVtPos.size())
		{
			YsVec3 o,n,itsc;
			YsPlane pln;

			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK && n!=YsOrigin()) ||
			   YsGetAverageNormalVector(n,plVtPos.size(),plVtPos.data())==YSOK)
			{
				o=plVtPos[0];
				pln.Set(o,n);
				if(pln.GetIntersection(itsc,from,dir)==YSOK && 0.0<=(itsc-from)*dir)
				{
					const YSSIDE side=YsCheckInsidePolygon3(itsc,plVtPos.size(),plVtPos.data());
					const double sqDist=(from-itsc).GetSquareLength();
					if((side==YSINSIDE || side==YSBOUNDARY) &&
					   (nullptr==candidatePlHd || sqDist<candidateDistance))
					{
						candidateDistance=sqDist;
						candidatePoint=itsc;
						candidatePlHd=plHd;
					}
				}
			}
		}
	}

	if(candidatePlHd!=NULL)
	{
		res.plHd=candidatePlHd;
		res.pos=candidatePoint;
	}

	return res;
}

YsShell::PolygonAndPos YsShell::FindNearestPolygon(const YsVec3 &from) const
{
	return FindNearestPolygonWithPrecisionFlag(from,YSFALSE);
}

YsShell::PolygonAndPos YsShell::FindNearestPolygonHighPrecision(const YsVec3 &from) const
{
	return FindNearestPolygonWithPrecisionFlag(from,YSTRUE);
}

YsShell::PolygonAndPos YsShell::FindNearestPolygonWithPrecisionFlag(const YsVec3 &from,YSBOOL highPrecision) const
{
	double nearDist=YsInfinity*YsInfinity;
	YsVec3 nearPos=YsVec3::Origin();

	YSBOOL first=YSTRUE;
	YsShellPolygonHandle nearPlHd=nullptr;
	YsArray <YsVec3,16> plVtPos;

	YSBOOL watch=YSFALSE;

	first=YSTRUE;

	for(auto plHd : AllPolygon())
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsVec3 plNom,vtPos,plCen;
		double dist;
		YsPlane pln;

		YSBOOL hit=YSFALSE;

		GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
		GetNormalOfPolygon(plNom,plHd);

		plVtPos.Set(nPlVt,nullptr);
		plCen=YsOrigin();
		for(int j=0; j<nPlVt; j++)
		{
			GetVertexPosition(vtPos,plVtHd[j]);
			dist=(vtPos-from).GetSquareLength();
			plVtPos[j]=vtPos;
			plCen+=vtPos;
			if(first==YSTRUE || dist<nearDist)
			{
				first=YSFALSE;
				hit=YSTRUE;

				nearDist=dist;
				nearPlHd=plHd;
				nearPos=vtPos;
			}
		}

		plCen/=(double)nPlVt;
		if(YSTRUE!=highPrecision && plNom!=YsOrigin())
		{
			pln.Set(plCen,plNom);
		}
		else
		{
			pln.MakeBestFitPlane(nPlVt,plVtPos);
		}

		YSRESULT haveNearPos=(YSTRUE==highPrecision ? pln.GetNearestPointHighPrecision(vtPos,from) : pln.GetNearestPoint(vtPos,from));
		if(haveNearPos==YSOK)
		{
			dist=(vtPos-from).GetSquareLength();
			if(YSTRUE==first || dist<nearDist)
			{
				YSSIDE side;
				if(nPlVt==3)
				{
					side=YsCheckInsideTriangle3(vtPos,plVtPos);
				}
				else if(plNom!=YsOrigin())
				{
					side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos,plNom);
				}
				else
				{
					side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos);
				}

				if(side==YSINSIDE || side==YSBOUNDARY)
				{
					first=YSFALSE;
					hit=YSTRUE;

					nearPlHd=plHd;
					nearPos=vtPos;
					nearDist=dist;
				}
			}
		}

		int prevJ;
		prevJ=nPlVt-1;
		for(int j=0; j<nPlVt; j++)
		{
			if(YsGetNearestPointOnLine3(vtPos,plVtPos[prevJ],plVtPos[j],from)==YSOK &&
			   YsCheckInBetween3(vtPos,plVtPos[prevJ],plVtPos[j])==YSTRUE)
			{
				dist=(vtPos-from).GetSquareLength();
				if(YSTRUE==first || dist<nearDist)
				{
					first=YSFALSE;
					hit=YSTRUE;

					nearPlHd=plHd;
					nearPos=vtPos;
					nearDist=dist;
				}
			}
			prevJ=j;
		}
	}

	PolygonAndPos res;
	res.pos=nearPos;
	res.plHd=nearPlHd;
	return res;
}

void YsShell::ShootRay(YsArray <YsVec3> &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	YsShellPolygonHandle plHd;
	int nVtx;
	YsVec3 vtx[256],p2;

	p2=p1+vec;

	intersect.Set(0,NULL);

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,i;
			YsPlane pln;
			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK) ||
			    YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(i,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(i,nVtx,vtx);
					if(side==YSINSIDE || side==YSBOUNDARY)
					{
						intersect.AppendItem(i);
					}
				}
			}
		}
	}
}


YSRESULT YsShell::BeginReadSrf(void)
{
	if(BeginMergeSrf()==YSOK)
	{
		CleanUp();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::BeginMergeSrf(void)
{
	if(readSrfVar!=NULL)
	{
		delete readSrfVar;
		readSrfVar=NULL;
	}

	readSrfVar=new YsShellReadSrfVariable;

	if(readSrfVar!=NULL)
	{
		return YSOK;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::BegimMergeSrf()");
		return YSERR;
	}
}

YSRESULT YsShell::ReadSrfOneLine
    (const char str[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	YSSIZE_T ac;

	if(str[0]=='#')  // Ignore comment line.
	{
		return YSOK;
	}

	YsArguments2(readSrfVar->args,str," \t","");
	ac=readSrfVar->args.GetN();
	if(ac>0)
	{
		if(readSrfVar->mode==0)  // Reading Vertices or other commands
		{
			YsVec3 v;
			YsShellVertexHandle vtHd;
			switch(str[0])
			{
			case 'S': // SURF
			case 's':
				break;
			case 'V': // VERTEX
			case 'v':
				v.Set(atof(readSrfVar->args[1]),atof(readSrfVar->args[2]),atof(readSrfVar->args[3]));
				vtHd=AddVertexH(v);
				readSrfVar->allVtHdList.Append(vtHd);
				if(ac>=5 && (readSrfVar->args[4][0]=='R' || readSrfVar->args[4][0]=='r') && roundVtx!=NULL)
				{
					roundVtx->Append(vtHd);
				}
				break;
			case 'E': // END
			case 'e':
				break;
			case 'F': // FACE
			case 'f':
				readSrfVar->mode=1;
				readSrfVar->plVtHdList.Set(0,NULL);
				readSrfVar->nom.Set(0.0,0.0,0.0);
				readSrfVar->noShadingPolygon=YSFALSE;
				readSrfVar->color.SetIntRGB(128,128,128);
				break;
			case 'M':
			case 'm':
				if(0==strcmp(readSrfVar->args[0],"MDT") || 0==strcmp(readSrfVar->args[0],"mdt") || 0==strcmp(readSrfVar->args[0],"Mdt"))
				{
					if(3<=ac)
					{
						YsString key(readSrfVar->args[1]),value(readSrfVar->args[2]);
						auto mdHd=AddMetaData(key,value);
					}
				}
				break;
			default:                // 2004/09/10
				return YSERR;       // 2004/09/10
			}
			return YSOK;
		}
		else // Reading Face
		{
			int i;
			switch(str[0])
			{
			case 'C': // COLOR
			case 'c':
				if(5<=ac)
				{
					readSrfVar->color.SetIntRGBA
					    (atoi(readSrfVar->args[1]),
					     atoi(readSrfVar->args[2]),
					     atoi(readSrfVar->args[3]),
					     atoi(readSrfVar->args[4]));
				}
				else if(4<=ac)
				{
					readSrfVar->color.SetIntRGB
					    (atoi(readSrfVar->args[1]),atoi(readSrfVar->args[2]),atoi(readSrfVar->args[3]));
				}
				else // if(ac<=2)
				{
					readSrfVar->color.Set15BitRGB(atoi(readSrfVar->args[1]));
				}

				break;
			case 'B':
			case 'b':
				readSrfVar->noShadingPolygon=YSTRUE;
				break;
			case 'N': // CENTER & NORMAL
			case 'n':
				if(ac==7)
				{
					readSrfVar->nom.Set(atof(readSrfVar->args[4]),atof(readSrfVar->args[5]),atof(readSrfVar->args[6]));
				}
				else if(ac==4)
				{
					readSrfVar->nom.Set(atof(readSrfVar->args[1]),atof(readSrfVar->args[2]),atof(readSrfVar->args[3]));
				}
				break;
			case 'V': // VERTEX
			case 'v':
				for(i=1; i<ac; i++)
				{
					const YSSIZE_T vtId=atoi(readSrfVar->args[i]);
					if(YSTRUE==readSrfVar->allVtHdList.IsInRange(vtId))
					{
						readSrfVar->plVtHdList.Append(readSrfVar->allVtHdList[vtId]);
					}
					else
					{
						YsString errMsg;
						errMsg.Printf("Vertex %d referred from a polygon doesn't exist.\n",(int)vtId);
						readSrfVar->PrintErrorMessage(errMsg);
					}
				}
				break;
			case 'E': // ENDFACE
			case 'e':
				if(3<=readSrfVar->plVtHdList.GetN())
				{
					readSrfVar->plHd=AddPolygonH(readSrfVar->plVtHdList.GetN(),readSrfVar->plVtHdList);
					readSrfVar->allPlHdList.Append(readSrfVar->plHd);
					SetNormalOfPolygon(readSrfVar->plHd,readSrfVar->nom);
					SetColorOfPolygon(readSrfVar->plHd,readSrfVar->color);
					if(noShadingPolygon!=NULL && readSrfVar->noShadingPolygon==YSTRUE)
					{
						noShadingPolygon->Append(readSrfVar->plHd);
					}
				}
				readSrfVar->mode=0;
				break;
			default:                // 2004/09/10
				return YSERR;       // 2004/09/10
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::EndReadSrf(void)
{
	if(readSrfVar!=NULL)
	{
		delete readSrfVar;
		readSrfVar=NULL;
		return YSOK;
	}
	return YSERR;
}


////////////////////////////////////////////////////////////

/* Deprecated.  Don't use */
YSBOOL YsCheckShellCollision(YsShell &sh1,YsShell &sh2)
{
	YSBOOL result;
	YsShellPolygonHandle plId1,plId2;
	int nv1,nv2;
	YsShellPolygon *pl1,*pl2;
	YsVec3 v1[256],nom1,v2[256],nom2;
	YsCollisionOfPolygon wizard;

	plId1=NULL;
	while((plId1=sh1.FindNextPolygon(plId1))!=NULL)
	{
		pl1=sh1.GetPolygon(plId1);
		pl1->freeAttribute3=0;
	}
	plId2=NULL;
	while((plId2=sh2.FindNextPolygon(plId2))!=NULL)
	{
		pl2=sh2.GetPolygon(plId2);
		pl2->freeAttribute3=0;
	}

	result=YSFALSE;

	plId1=NULL;
	while((plId1=sh1.FindNextPolygon(plId1))!=NULL)
	{
		nv1=sh1.GetNumVertexOfPolygon(plId1);
		sh1.GetVertexListOfPolygon(v1,256,plId1);
		pl1=sh1.GetPolygon(plId1);
		sh1.GetNormalOfPolygon(nom1,plId1);

		if(sh1.TrustPolygonNormal()==YSTRUE && nom1!=YsOrigin())
		{
			wizard.SetPolygon1(nv1,v1,nom1);
		}
		else
		{
			wizard.SetPolygon1(nv1,v1);
		}

		plId2=NULL;
		while((plId2=sh2.FindNextPolygon(plId2))!=NULL)
		{
			nv2=sh2.GetNumVertexOfPolygon(plId2);
			sh2.GetVertexListOfPolygon(v2,256,plId2);
			sh2.GetNormalOfPolygon(nom2,plId2);

			if(sh2.TrustPolygonNormal()==YSTRUE && nom2!=YsOrigin())
			{
				wizard.SetPolygon2(nv2,v2,nom2);
			}
			else
			{
				wizard.SetPolygon2(nv2,v2);
			}

			if(wizard.CheckCollision()==YSTRUE)
			{
				pl2=sh2.GetPolygon(plId2);
				pl1->freeAttribute3=1;
				pl2->freeAttribute3=1;
				result=YSTRUE;
			}
		}
	}

	return result;
}


YSBOOL YsCheckShellCollisionEx(
    YsVec3 &firstFound,YsShellPolygonHandle &foundPlHd1,YsShellPolygonHandle &foundPlHd2,
    const YsShell &shl1,const YsShell &shl2)
{
	YSBOOL result;
	YsShellPolygonHandle plHd1,plHd2;
	int nv1,nv2;
	YsArray <YsVec3,256> v1,v2;
	YsVec3 nom1,nom2;
	YsCollisionOfPolygon wizard;

	firstFound=YsOrigin();
	foundPlHd1=NULL;
	foundPlHd2=NULL;

	result=YSFALSE;

	plHd1=NULL;
	while((plHd1=shl1.FindNextPolygon(plHd1))!=NULL)
	{
		nv1=shl1.GetNumVertexOfPolygon(plHd1);
		shl1.GetVertexListOfPolygon(v1,plHd1);
		shl1.GetNormalOfPolygon(nom1,plHd1);

		if(shl1.TrustPolygonNormal()==YSTRUE && nom1!=YsOrigin())
		{
			wizard.SetPolygon1(nv1,v1,nom1);
		}
		else
		{
			wizard.SetPolygon1(nv1,v1);
		}

		plHd2=NULL;
		while((plHd2=shl2.FindNextPolygon(plHd2))!=NULL)
		{
			nv2=shl2.GetNumVertexOfPolygon(plHd2);
			shl2.GetVertexListOfPolygon(v2,plHd2);
			shl2.GetNormalOfPolygon(nom2,plHd2);

			if(shl2.TrustPolygonNormal()==YSTRUE && nom2!=YsOrigin())
			{
				wizard.SetPolygon2(nv2,v2,nom2);
			}
			else
			{
				wizard.SetPolygon2(nv2,v2);
			}

			if(wizard.CheckCollision(firstFound)==YSTRUE)
			{
				foundPlHd1=plHd1;
				foundPlHd2=plHd2;
				result=YSTRUE;
			}
		}
	}

	return result;
}





//####
// Future modification
//   1. Just throw all vertices/polygons to one shell
//   2. Slash self-intersecting edges
//   3. Decompose polygons
// Advantage
//   Can guarantee winged-edge structure
// Disadvantage
//   Again how to take care of overlapping polygons?

// Another solution
//   Right after blending shell, before identical vertices are deleted,
//   Mark floating vertices and make tables which edge a floating vertex is from
//   Then, replace edges one by one refering to the table

// Maybe this will be the best solution
//   1. Dump all vertices/polygons of two shells into one shell. Keep track of which polygon is from which
//   2. Slash polygons that are from Polygon 1{2} by shell 2{1}
//      But, make use of YsSwordNewVertexLog.
//      Create new vertices exactly the same manner that is written in NewVertexLog
//      New vertex ids are initially negative. Update them when they are added to the shell.
//      (New function must be added in YsSword class. UpdateNewVertexId(int temporaryId,int Id);
//         Note1:Ids are obtained by YsShell::GetSearchKey() function
//         Note2:Update both IDs in NewVertexLog and VTIDs in YsSwordPolygon
//         Note3:If an edge betweenVt1-betweenVt2 of NewVertexLog doesn't exist, it means
//               the edge is interior of the slashed polygon. So, just add a floating vertex)
//      After steps above, all vertices in YsSword is in the shell.
//      Remove original polygon. Add new polygons in YsSword class. No need to add new vertex.
//   3. Overlapping Polygons : Take one of two from Shell 1 (or 2)

static YSRESULT YsBlendShell1to2
    (YsShell &newShell,
     const YsShell &sh1,const YsShell &sh2,int f,YSBOOL elimOverlap)
{
	int i,j,np1,np2,nv1,nv2;
	YsShellPolygonHandle plHd;
	const YsShellPolygon *pl1;
	YsShellPolygon *plNew;
	YsVec3 v1[256],v2[256];
	int vtId[256];
	YsCollisionOfPolygon wizard;
	YsSword sword;

	sh1.Encache();
	sh2.Encache();

	np1=sh1.GetNumPolygon();
	np2=sh2.GetNumPolygon();

	for(i=0; i<np1; i++)
	{
		nv1=sh1.GetNumVertexOfPolygon(i);
		sh1.GetVertexListOfPolygon(v1,256,i);
		sh1.GetVertexListOfPolygon(vtId,256,i);
		pl1=sh1.GetPolygon(i);
		wizard.SetPolygon1(nv1,v1);
		sword.SetInitialPolygon(nv1,v1,vtId);


		// Future expansion ####
		//   Use 3D lattice to speed up intersection


		for(j=0; j<np2; j++)
		{
			nv2=sh2.GetNumVertexOfPolygon(j);
			sh2.GetVertexListOfPolygon(v2,256,j);
			wizard.SetPolygon2(nv2,v2);

			if(wizard.CheckOnTheSamePlane()==YSTRUE)
			{
				sword.SlashByOverlappingPolygon3(nv2,v2,j);
			}
			else if(wizard.CheckCollision()==YSTRUE)
			{
				sword.Slash(nv2,v2);
			}
		}

		// Future expansion ####
		//   Refer to YsSwordNewVertexLog and YsShellEdgeList,
		//   Add vertices to edges of other polygons -> guarantee winged edge structure

		int nPiece;
		const YsArray <YsVec3> *plg;
		const YsArray <int> *vtId;

		nPiece=sword.GetNumPolygon();
		for(j=0; j<nPiece; j++)
		{
			int slashedPlId;
			if(sword.IsFromOverlappingPolygon3(j,slashedPlId)!=YSTRUE)
			{
				plg=sword.GetPolygon(j);
				vtId=sword.GetVertexIdList(j);
				plHd=newShell.AddPolygonVertex(plg->GetNumItem(),plg->GetArray(),f,vtId->GetArray());
				plNew=newShell.GetPolygon(plHd);
				plNew->SetColor(pl1->GetColor());
				plNew->SetNormal(pl1->GetNormal());
				plNew->freeAttribute2=f;
				plNew->freeAttribute3=i;
				plNew->freeAttribute4=0;
			}
			else if(elimOverlap!=YSTRUE)
			{
				plg=sword.GetPolygon(j);
				plHd=newShell.AddPolygonVertexH(plg->GetNumItem(),plg->GetArray());
				plNew=newShell.GetPolygon(plHd);
				plNew->SetColor(pl1->GetColor());
				plNew->SetNormal(pl1->GetNormal());
				plNew->freeAttribute2=2;   //2 means shared polygon
				plNew->freeAttribute3=i;           // PlId of Shell[0]
				plNew->freeAttribute4=slashedPlId; // PlId of Shell[1]
			}
		}
	}
	return YSOK;
}

YSBOOL YsShell::EliminateHangnail(int &nv,YsShellVertexHandle v[])
// Idea : If edges are going like  A-B,B-C,C-B,B-D, then edge B-C,C-B can be omitted.
{
	for(int i=0; i<nv; i++)
	{
		int j,k,l;
		j=(i+1)%nv;
		k=(i+2)%nv;
		if(v[i]==v[k])
		{
			if(j>k)
			{
				int a;
				a=j;
				j=k;
				k=a;
			}
			for(l=k; l<nv; l++)
			{
				v[l]=v[l+1];
			}
			nv--;
			for(l=j; l<nv; l++)
			{
				v[l]=v[l+1];
			}
			nv--;
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSHASHKEY YsShell::GetUniqueSearchKey(void)
{
	return uniqueSearchKey++;
}

void YsShell::RewindSearchKey(void)
{
	uniqueSearchKey=1;  // Leave 0 unused.
}

// Note: vtIdNew must not be equal to vtIdOrg
YSRESULT YsShell::BlendPolygonVtId
    (int &nvNew,YsShellVertexHandle vtIdNew[],int nBuf,
     int nv1,const YsShellVertexHandle vtId1[],int nv2,const YsShellVertexHandle vtId2[])
{
	if(nBuf<nv2+nv1)
	{
		return YSERR;
	}

	int i;
	YsShellVertexHandle vtOrg1,vtOrg2,vt1,vt2;

	for(i=0; i<nv1; i++)
	{
		vtOrg1=vtId1[i];
		vtOrg2=vtId1[(i+1)%nv1];
		for(int j=0; j<nv2; j++)
		{
			vt1=vtId2[j];
			vt2=vtId2[(j+1)%nv2];
			if(vt1==vtOrg1 && vt2==vtOrg2)
			{
				int n;

				n=0;
				for(int k=0; k<nv1; k++)
				{
					vtIdNew[n++]=vtId1[(i+1+k)%nv1];
				}
				for(int l=1; l<nv2-1; l++)
				{
					vtIdNew[n++]=vtId2[(j+nv2-l)%nv2];
				}
				nvNew=n;

				while(EliminateHangnail(nvNew,vtIdNew)==YSTRUE)
				{
				}

				return YSOK;
			}
			else if(vt1==vtOrg2 && vt2==vtOrg1)
			{
				int n;

				n=0;
				for(int k=0; k<nv1; k++)
				{
					vtIdNew[n++]=vtId1[(i+1+k)%nv1];
				}
				for(int l=1; l<nv2-1; l++)
				{
					vtIdNew[n++]=vtId2[(j+1+l)%nv2];
				}
				nvNew=n;

				while(EliminateHangnail(nvNew,vtIdNew)==YSTRUE)
				{
				}

				return YSOK;
			}
		}
	}
	return YSERR;
}


YSRESULT YsShell::OmitPolygonAfterBlend(void)
//Idea : If two polygons are originated from the identical polygon, no need to remain separated
{
	YsOneMemoryBlock <YsShellVertexHandle,8192> memBlock;
	int nVtxNew,nVtxOfShell;
	YsShellVertexHandle *vtxNew;
	int stopInifiniteLoop;
	YsShellPolygonHandle plHd1,plHd2;

	stopInifiniteLoop=256;

	nVtxOfShell=GetNumVertex();
	vtxNew=memBlock.GetMemoryBlock(nVtxOfShell);

REPEAT:

	plHd1=NULL;
	while((plHd1=FindNextPolygon(plHd1))!=NULL)
	{
		plHd2=plHd1;
		while((plHd2=FindNextPolygon(plHd2))!=NULL)
		{
			YsShellPolygon *plg1,*plg2;
			int nVtx1;
			const YsShellVertexHandle *vtx1;
			int nVtx2;
			const YsShellVertexHandle *vtx2;

			plg1=GetPolygon(plHd1);
			nVtx1=plg1->GetNumVertex();
			vtx1=plg1->GetVertexArray();

			plg2=GetPolygon(plHd2);
			nVtx2=plg2->GetNumVertex();
			vtx2=plg2->GetVertexArray();

			if(plg1!=NULL && plg2!=NULL &&
               plg1->freeAttribute2==plg2->freeAttribute2 &&
               plg1->freeAttribute3==plg2->freeAttribute3)
			{
				if(BlendPolygonVtId(nVtxNew,vtxNew,nVtxOfShell,nVtx1,vtx1,nVtx2,vtx2)==YSOK)
				{
					if(ModifyPolygon(plHd1,nVtxNew,vtxNew)==YSOK)
					{
						DeletePolygon(plHd2);
					}

					if(stopInifiniteLoop>0)
					{
						stopInifiniteLoop--;
						goto REPEAT;
					}
					else
					{
						goto FINISH;
					}
				}
			}
		}
	}

FINISH:

	return YSOK;
}

void YsShell::SetMatrix(const YsMatrix4x4 &m)
{
	mat=m;
	matrixIsSet=YSTRUE;
	bbxIsValid=YSFALSE;
}

YSRESULT YsShell::Triangulate
    (YsArray <YsShellPolygonHandle> &newPlHdList,YsShellPolygonHandle plHd,YSBOOL killOriginalPolygon)
{
	newPlHdList.Set(0,NULL);

	int i,np;
	np=GetNumVertexOfPolygon(plHd);
	if(np>3)
	{
		YsVec3 nom;
		YsColor col;
		YsSword swd;
		YsArray <YsVec3,256> plVtPos;
		YsArray <YsShellVertexHandle,256> plVtHd;
		YsArray <int,256> key;
		const YsArray <int> *vtKeyList;
		YsShellPolygonHandle newPlHd;

		plVtHd.Set(np,GetVertexListOfPolygon(plHd));
		plVtPos.Set(np,NULL);
		key.Set(np,NULL);

		GetNormalOfPolygon(nom,plHd);
		GetColorOfPolygon(col,plHd);

		if(GetVertexListOfPolygon(plVtPos,np,plHd)==YSOK)
		{
			for(i=np-1; i>0; i--)
			{
				if(plVtHd[i]==plVtHd[i-1])
				{
					plVtHd.Delete(i);
					plVtPos.Delete(i);
					np--;
				}
			}
			if(plVtHd.GetN()>=3 && plVtHd.GetTopItem()==plVtHd.GetEndItem())
			{
				plVtHd.Delete(np-1);
				plVtPos.Delete(np-1);
				np--;
			}

			if(np<3)
			{
				if(killOriginalPolygon==YSTRUE)
				{
					DeletePolygon(plHd);
				}
				return YSOK;
			}

			for(i=0; i<np; i++)
			{
				key[i]=i;  // GetSearchKey(plVtHd[i]);
			}
			swd.SetInitialPolygon(np,plVtPos,key);
			if(swd.Triangulate()==YSOK)
			{
				YSRESULT res=YSOK;
				for(i=0; i<swd.GetNumPolygon(); i++)
				{
					vtKeyList=swd.GetVertexIdList(i);
					if(vtKeyList->GetNumItem()!=3)
					{
						YsErrOut("YSRESULT YsShell::Triangulate()\n");
						YsErrOut("  Non-triangular polygon remains.\n");
						res=YSERR;
						break;
					}

					YsShellVertexHandle newPlVtHd[3];
					newPlVtHd[0]=plVtHd[(*vtKeyList)[0]]; // search->FindVertex(*this,vtKeyList->GetItem(0));
					newPlVtHd[1]=plVtHd[(*vtKeyList)[1]]; // search->FindVertex(*this,vtKeyList->GetItem(1));
					newPlVtHd[2]=plVtHd[(*vtKeyList)[2]]; // search->FindVertex(*this,vtKeyList->GetItem(2));
					newPlHd=AddPolygonH(3,newPlVtHd);
					SetColorOfPolygon(newPlHd,col);
					SetNormalOfPolygon(newPlHd,nom);
					GetPolygon(newPlHd)->CopyAttribute(*GetPolygon(plHd)); // 2004/01/12
					newPlHdList.AppendItem(newPlHd);
				}

				if(res==YSOK)
				{
					if(killOriginalPolygon==YSTRUE)
					{
						DeletePolygon(plHd);
					}
				}
				else
				{
					for(i=0; i<newPlHdList.GetNumItem(); i++)
					{
						DeletePolygon(newPlHdList[i]);
					}
				}

				return res;
			}
			else
			{
				YsErrOut("YSRESULT YsShell::Triangulate()\n");
				YsErrOut("  Triangulation failed.\n");
				return YSERR;
			}
		}

		YsErrOut("YSRESULT YsShell::Triangulate()\n");
		YsErrOut("  Cannot retrieve geometry of the polygon.\n");
		return YSERR;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

// Added 2000/11/10
void YsShell::SetTrustPolygonNormal(YSBOOL trust)
{
	trustPolygonNormal=trust;
}

// Added 2000/11/10
YSBOOL YsShell::TrustPolygonNormal(void) const
{
	return trustPolygonNormal;
}

////////////////////////////////////////////////////////////

// Added 2001/03/21

YSRESULT YsShell::FreezeVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *v;
	v=GetVertex(vtHd);
	if(v!=NULL)
	{
		if(v->GetReferenceCount()>0)
		{
			YsErrOut("YsShell::FreezeVertex\n  Tried to delete a Vertex in use\n");
			return YSERR;
		}

		if(searchTable!=NULL)
		{
			searchTable->DeleteVertex(*this,vtHd);
		}

		vtx.Freeze(vtHd);

		bbxIsValid=YSFALSE;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::FreezePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *p;
	YsShellVertex *vtx;
	p=GetPolygon(plHd);

	if(p!=NULL)
	{
		int i,nVtx;

		nVtx=GetNumVertexOfPolygon(plHd);
		for(i=0; i<nVtx; i++)
		{
			vtx=GetVertex(p->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		plg.Freeze(plHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::MeltVertex(YsShellVertexHandle vtHd)
{
	if(vtHd!=NULL)
	{
		if(YSTRUE==vtx.IsFrozen(vtHd))
		{
			vtx.Melt(vtHd);
			if(searchTable!=NULL)
			{
				searchTable->AddVertex(*this,vtHd);
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsShell::MeltVertex()\n  Not a vertex of the shell.\n");
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::MeltVertex()\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
}

YSRESULT YsShell::MeltPolygon(YsShellPolygonHandle plHd)
{
	if(plHd!=NULL)
	{
		if(YSTRUE==plg.IsFrozen(plHd))
		{
			plg.Melt(plHd);

			auto plgPtr=plg[plHd];
			for(int i=0; i<plgPtr->GetNumVertex(); i++)
			{
				GetVertex(plgPtr->GetVertexHandle(i))->IncrementReferenceCount();
			}

			if(searchTable!=NULL)
			{
				searchTable->AddPolygon(*this,plHd);
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsShell::MeltPolygon()\n  Not a vertex of the shell.\n");
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::MeltPolygon()\n  NULL pointer is passed.\n");
		return YSERR;
	}
}

YSRESULT YsShell::DeleteFrozenVertex(YsShellVertexHandle vtHd)
{
	if(vtHd!=NULL && YSTRUE==vtx.IsFrozen(vtHd))
	{
		auto vtxPtr=vtx.GetObjectMaybeFrozen(vtHd);
		DetachSpecialAttributeBeforeDeletingVertex(*vtxPtr);
		return vtx.DeleteFrozen(vtHd);
	}
	return YSERR;
}

YSRESULT YsShell::DeleteFrozenPolygon(YsShellPolygonHandle plHd)
{
	if(plHd!=NULL && YSTRUE==plg.IsFrozen(plHd))
	{
		auto plgPtr=plg.GetObjectMaybeFrozen(plHd);
		DetachSpecialAttributeBeforeDeletingPolygon(*plgPtr);
		return plg.DeleteFrozen(plHd);
	}
	return YSERR;
}

YSRESULT YsShell::DisposeFrozenPolygon(void)
{
	for(PolygonHandle plHd=nullptr; YSOK==plg.MoveToNextIncludingFrozen(plHd); )
	{
		if(YSTRUE==plg.IsFrozen(plHd))
		{
			DetachSpecialAttributeBeforeDeletingPolygon(*plg.GetObjectMaybeFrozen(plHd));
			plg.DeleteFrozen(plHd);
		}
	}
	return YSOK;
}

YSRESULT YsShell::DisposeFrozenVertex(void)
{
	for(VertexHandle vtHd=nullptr; YSOK==vtx.MoveToNextIncludingFrozen(vtHd); )
	{
		if(YSTRUE==vtx.IsFrozen(vtHd))
		{
			DetachSpecialAttributeBeforeDeletingVertex(*vtx.GetObjectMaybeFrozen(vtHd));
			vtx.DeleteFrozen(vtHd);
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

// Added 2000/11/20

YsShellSpecialAttribute *YsShell::GetSpecialAttributeVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YsShellSpecialAttribute *YsShell::GetSpecialAttributeVertexMaybeFrozen(YsShellVertexHandle vtHd)
{
	YsShellVertex *vtx;
	vtx=GetVertexMaybeFrozen(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

const YsShellSpecialAttribute *YsShell::GetSpecialAttributeVertex(YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YsShellSpecialAttribute *YsShell::GetSpecialAttributePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YsShellSpecialAttribute *YsShell::GetSpecialAttributePolygonMaybeFrozen(YsShellPolygonHandle plHd)
{
	YsShellPolygon *plg;
	plg=GetPolygonMaybeFrozen(plHd);
	if(plg!=NULL)
	{
		return plg->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

const YsShellSpecialAttribute *YsShell::GetSpecialAttributePolygon(YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShell::SetSpecialAttributeVertex(YsShellVertexHandle vtHd,YsShellSpecialAttribute *attr)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		vtx->SetSpecialAttribute(attr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetSpecialAttributePolygon(YsShellPolygonHandle plHd,YsShellSpecialAttribute *attr)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->SetSpecialAttribute(attr);
		return YSOK;
	}
	return YSERR;
}



YSRESULT YsShell::AttachSpecialAttributeToNewVertex(YsShellVertex &,YsShellVertexHandle)
{
	// Sub-class must create a new special attribute
	// and attach to vtx if necessary.
	return YSOK;
}

YSRESULT YsShell::AttachSpecialAttributeToNewPolygon(YsShellPolygon &,YsShellPolygonHandle)
{
	// Sub-class must create a new special attribute
	// and attach to plg if necessary.
	return YSOK;
}

YSRESULT YsShell::DetachSpecialAttributeBeforeDeletingVertex(YsShellVertex &)
{
	// Sub-class must get special attribute from vtx,
	// delete and call vtx.SpecialAttributeDetached();
	return YSOK;
}

YSRESULT YsShell::DetachSpecialAttributeBeforeDeletingPolygon(YsShellPolygon &)
{
	// Sub-class must get special attribute from plg,
	// delete and call vtx.SpecialAttributeDetached();
	return YSOK;
}


////////////////////////////////////////////////////////////

YSRESULT YsShell::BuildOctree(YsShellOctree &oct,int maxDepth,double bbxMargine) const
{
	YsOneMemoryBlock <YsVec3,64> memBlk;
	int nPlVt,maxNumPlVt;
	YsVec3 *plVt;
	int nPl,plId;
	YsLoopCounter ctr;

	Encache();

	YsVec3 max,min,cen;
	GetBoundingBox(max,min);

	cen=(min+max)/2.0;
	min=cen+(min-cen)*bbxMargine;
	max=cen+(max-cen)*bbxMargine;

	oct.CleanUp();
	oct.shl=this;
	oct.SetDomain(max,min,YSUNKNOWNSIDE);

	maxNumPlVt=GetMaxNumVertexOfPolygon();
	plVt=memBlk.GetMemoryBlock(maxNumPlVt);

	YsPrintf("Building Octree\n");
	YsPrintf("  Marking boundary nodes\n");

	nPl=GetNumPolygon();
	ctr.BeginCounter(nPl);
	for(plId=0; plId<nPl; plId++)
	{
		ctr.Increment();

		nPlVt=GetNumVertexOfPolygon(plId);
		if(nPlVt>0 && GetVertexListOfPolygon(plVt,maxNumPlVt,plId)==YSOK)
		{
			oct.SubdivideByPolygon(nPlVt,plVt,maxDepth,YSBOUNDARY,YSUNKNOWNSIDE);
		}
	}
	ctr.EndCounter();

	YsPrintf("  Marking Outside/Inside\n");
	oct.GoTraverse(YSTRUE,-1,0);  // Parameter param1==-1 means mark outside/inside
	YsPrintf("  Done.\n");

	return YSOK;
}

YSBOOL YsShell::IsStarShape(const YsVec3 &pt) const
{
	// 1.The ray from pt to each vertex must not intersect with other polygons except at end points
	// 2.All triangles formed by pt and each edge must not intersect with another edge except at end points

	YsArray <int> freeAttrib4Save;

	YsShellEdgeList defEdgeList;
	const YsShellEdgeList *edgeList;

	SaveFreeAttribute4OfVertex(freeAttrib4Save);

	// if(supplyEdgeList!=NULL)
	// {
	// 	edgeList=supplyEdgeList;
	// }
	// else
	{
		defEdgeList.Build(*this);
		edgeList=&defEdgeList;
	}

	int i,j;
	int nEdg,edId;
	YsShellVertexHandle vtHd,edVtHd[2];
	YsVec3 tri[3],edg[2],intersection;
	YsPlane pln;
	YsArray <YsVec3> intersectionList;

	nEdg=edgeList->GetNumEdge();

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *vtx;
		vtx=(YsShellVertex *)GetVertex(vtHd);
		vtx->freeAttribute4=0;
	}

	for(edId=0; edId<nEdg; edId++)
	{
		if(edgeList->GetEdge(edVtHd[0],edVtHd[1],edId)==YSOK)
		{
			for(i=0; i<2; i++)
			{
				YsShellVertex *vtx;
				GetVertexPosition(edg[i],edVtHd[i]);
				vtx=(YsShellVertex *)GetVertex(edVtHd[i]);
				if(vtx->freeAttribute4==0)
				{
					vtx->freeAttribute4=1;
					ShootRay(intersectionList,pt,edg[i]-pt);
					for(j=0; j<intersectionList.GetNumItem(); j++)
					{
						if(intersectionList.GetItem(j)!=pt && intersectionList.GetItem(j)!=edg[i])
						{
							RestoreFreeAttribute4OfVertex(freeAttrib4Save);
							return YSFALSE;
						}
					}
				}

			}

			tri[0]=edg[0];
			tri[1]=edg[1];
			tri[2]=pt;
			pln.MakePlaneFromTriangle(tri[0],tri[1],tri[2]);
			for(i=0; i<nEdg; i++)
			{
				edgeList->GetEdge(edVtHd[0],edVtHd[1],i);
				GetVertexPosition(edg[0],edVtHd[0]);
				GetVertexPosition(edg[1],edVtHd[1]);
				if(pln.GetIntersection(intersection,edg[0],edg[1]-edg[0])==YSOK &&
				   YsCheckInBetween3(intersection,edg[0],edg[1])==YSTRUE &&
				   YsCheckInsidePolygon3(intersection,3,tri)!=YSOUTSIDE &&
				   tri[0]!=intersection &&
				   tri[1]!=intersection &&
				   tri[2]!=intersection)
				{
					RestoreFreeAttribute4OfVertex(freeAttrib4Save);
					return YSFALSE;
				}
			}
		}
		else
		{
			YsErrOut("YsShell::IsStarShape()\n");
			YsErrOut("  Conflict in the edgelist\n");
		}
	}

	RestoreFreeAttribute4OfVertex(freeAttrib4Save);
	return YSTRUE;
}

YSRESULT YsShell::AutoComputeNormal(YsShellPolygonHandle plHd,YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal)
{
	int nVtx;
	YsVec3 oldNom;
	YsArray <YsVec3,16> vtxList;

	if(plHd!=NULL && (nVtx=GetNumVertexOfPolygon(plHd))>=3)
	{
		vtxList.Set(nVtx,NULL);
		GetVertexListOfPolygon(vtxList.GetEditableArray(),nVtx,plHd);

		GetNormalOfPolygon(oldNom,plHd);
		if(oldNom==YsOrigin() || replaceCurrentNormal==YSTRUE)
		{
			YsVec3 nom;
			if(YsGetAverageNormalVector(nom,nVtx,vtxList.GetArray())==YSOK)
			{
				if(nom.IsNormalized()!=YSTRUE)
				{
					nom.Normalize();
				}

				if(ignoreCurrentNormal!=YSTRUE && oldNom!=YsOrigin() && nom*oldNom<0.0)
				{
					nom=-nom;
				}

				SetNormalOfPolygon(plHd,nom);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell::AutoComputeNormalAll(YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal)
{
	YSRESULT res;
	YsShellPolygonHandle plHd;

	res=YSOK;
	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		if(AutoComputeNormal(plHd,replaceCurrentNormal,ignoreCurrentNormal)!=YSOK)
		{
			res=YSERR;
		}
	}
	return res;
}

YSRESULT YsShell::AutoComputeVertexNormalAll(YSBOOL weighted)
{
#ifndef YSLOWMEM
	YsShellVertexHandle vtHd;
	int i,nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsShellPolygonHandle plHd;
	YsVec3 plNom,vtNom;
	YsVec3 tri[3],v[2];  // for weighted==YSTRUE

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		GetVertex(vtHd)->SetNormal(YsOrigin());
	}

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

		GetNormalOfPolygon(plNom,plHd);
		for(i=0; i<nPlVt; i++)
		{
			vtNom=GetVertex(plVtHd[i])->GetNormal();
			if(weighted==YSTRUE)
			{
				GetVertexPosition(tri[0],plVtHd[(i+nPlVt-1)%nPlVt]);
				GetVertexPosition(tri[1],plVtHd[i]);
				GetVertexPosition(tri[2],plVtHd[(i+1)%nPlVt]);
				v[0]=tri[1]-tri[0];
				v[1]=tri[2]-tri[1];
				if(v[0].Normalize()==YSOK && v[1].Normalize()==YSOK)
				{
					vtNom+=acos(-v[0]*v[1])*plNom;
				}
			}
			else
			{
				vtNom+=plNom;
			}
			GetVertex(plVtHd[i])->SetNormal(vtNom);
		}
	}

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		vtNom=GetVertex(vtHd)->GetNormal();
		vtNom.Normalize();
		GetVertex(vtHd)->SetNormal(vtNom);
	}

	return YSOK;
#else
	return YSERR;
#endif
}

/*static*/ void YsShell::CleanUpVertexPolygonStore(void)
{
	// Left for backward compatibility.  This does nothing.
}

void YsShell::CollectGarbage(void) const
{
	// vertexStore.CollectGarbage();
	// polygonStore.CollectGarbage();
}

YsShell::PolygonHandle YsShell::CreatePolygon(void)
{
	PolygonHandle plHd;
	plHd=plg.Create();
	plg[plHd]->Initialize();
	AttachSpecialAttributeToNewPolygon(*plg[plHd],plHd);
	return plHd;
}

YsShell::VertexHandle YsShell::CreateVertex(void)
{
	VertexHandle vtHd;
	vtHd=vtx.Create();
	vtx[vtHd]->Initialize();
	vtx[vtHd]->searchKey=vtx.GetSearchKey(vtHd);
	AttachSpecialAttributeToNewVertex(*vtx[vtHd],vtHd);
	return vtHd;
}

void YsShell::ResetVertexMapping(void)
{
	vtxMapping.CleanUp();
}

YsShellVertexHandle YsShell::FindVertexMapping(const YsShellVertex *fromVtx) const
{
	YsShellVertexHandle foundVtHd;
	if(YSOK==vtxMapping.FindElement(foundVtHd,fromVtx->searchKey))
	{
		return foundVtHd;
	}
	return NULL;
}

YsShellVertexHandle YsShell::FindVertexMapping(YsShellVertexHandle vtHd) const
{
	YsShellVertexHandle foundVtHd;
	if(YSOK==vtxMapping.FindElement(foundVtHd,GetSearchKey(vtHd)))
	{
		return foundVtHd;
	}
	return NULL;
}

YSRESULT YsShell::AddVertexMapping(YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd) // fromVtHd needs to be the vertex of this shell
{
	if(NULL==toVtHd)
	{
		return vtxMapping.DeleteElementIfExist(GetSearchKey(fromVtHd));
	}
	else
	{
		return vtxMapping.UpdateElement(GetSearchKey(fromVtHd),toVtHd);
	}
}

YSRESULT YsShell::AddVertexMapping(const YsShellVertex *fromVtx,YsShellVertexHandle toVtHd) // fromVtHd needs to be the vertex of this shell
{
	if(NULL==toVtHd)
	{
		return vtxMapping.DeleteElementIfExist(fromVtx->searchKey);
	}
	else
	{
		return vtxMapping.UpdateElement(fromVtx->searchKey,toVtHd);
	}
}

////////////////////////////////////////////////////////////

YSRESULT YsShell3dTree::Build(const class YsShell &shl,const class YsShellVertexStore *limitedVtx,const class YsShellVertexStore *excludedVtx)
{
	int i,a,nVtx;
	YsArray <YsShellVertexHandle> vtHdList;
	YsShellVertexHandle vtHd;
	YsLoopCounter ctr;

	nVtx=shl.GetNumVertex();
	if(nVtx>0)
	{
		PreAllocateNode(nVtx);

		vtHdList.Set(nVtx,NULL);

		i=0;
		vtHd=NULL;
		while((vtHd=shl.FindNextVertex(vtHd))!=NULL)
		{
			if((NULL!=limitedVtx && YSTRUE!=limitedVtx->IsIncluded(vtHd)) ||
			   (NULL!=excludedVtx && YSTRUE==excludedVtx->IsIncluded(vtHd)))
			{
				continue;
			}

			vtHdList[i]=vtHd;
			i++;
		}
		vtHdList.Resize(i);
		nVtx=i;

		srand((int)time(NULL));

		YsPrintf("Building KDTree\n");
		YsPrintf("  Shuffling\n");
		ctr.BeginCounter(vtHdList.GetN());
		for(i=0; i<vtHdList.GetN(); i++)
		{
			ctr.Increment();

			YsShellVertexHandle vtHd1,vtHd2;

			a=rand()%vtHdList.GetN();
			vtHd1=vtHdList.GetItem(i);
			vtHd2=vtHdList.GetItem(a);

			vtHdList.SetItem(i,vtHd2);
			vtHdList.SetItem(a,vtHd1);
		}
		ctr.EndCounter();

		YsPrintf("  Adding Node Points\n");
		ctr.BeginCounter(vtHdList.GetN());
		for(i=0; i<vtHdList.GetN(); i++)
		{
			ctr.Increment();

			YsVec3 pos;

			vtHd=vtHdList.GetItem(i);
			shl.GetVertexPosition(pos,vtHd);
			AddPoint(pos,vtHd);
		}
		ctr.EndCounter();
		YsPrintf("  Done\n");
	}

	return YSOK;
}

YSRESULT YsShell3dTree::AddVertex(const class YsShell &shl,YsShellVertexHandle vtHd)
{
	YsVec3 pos;
	shl.GetVertexPosition(pos,vtHd);
	return AddPoint(pos,vtHd);
}

class YsShellKdTreeEnumParam : public YsKdTreeEnumParam
{
public:
	YsArray <YsShellVertexHandle,128> *vtxList;
};

YSRESULT YsShell3dTree::MakeVertexList(YsArray <YsShellVertexHandle,128> &lst,const YsVec3 &min,const YsVec3 &max) const
{
	YsShellKdTreeEnumParam enumParam;
	lst.Set(0,NULL);
	enumParam.vtxList=&lst;
	return Traverse(&enumParam,min,max);
}

YSRESULT YsShell3dTree::Enumerate(YsKdTreeEnumParam *enumParam,const YsVec3 &,const YsShellVertexHandle &attrib) const
{
	((YsShellKdTreeEnumParam *)enumParam)->vtxList->AppendItem(attrib);
	return YSOK;
}


////////////////////////////////////////////////////////////

YsShellEdgeList::YsShellEdgeList()
{
	nEdge=0;
	edgeList=NULL;
	shell=NULL;
}

YsShellEdgeList::YsShellEdgeList(const YsShellEdgeList &from)
{
	*this=from;

	nEdge=from.nEdge;
	int i;
	for(i=0; i<8; i++)
	{
		edgeHash[i]=from.edgeHash[i];
	}
	edgeList=from.edgeList->Duplicate();
}

YsShellEdgeList::~YsShellEdgeList()
{
	if(edgeList!=NULL)
	{
		edgeList->DeleteList();
	}
}

int YsShellEdgeList::GetNumEdge(void) const
{
	return nEdge;
}

YSRESULT YsShellEdgeList::CleanUp(void)
{
	int i;

	for(i=0; i<HashTableSize; i++)
	{
		edgeHash[i].Set(0,NULL);
	}

	edgeList->DeleteList();

	nEdge=0;
	edgeList=NULL;
	shell=NULL;

	return YSOK;
}

YSRESULT YsShellEdgeList::Build(const YsShell &shl)
{
	YsShellPolygonHandle plHd;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	shell=&shl;

	plHd=NULL;
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		nPlVt=shl.GetNumVertexOfPolygon(plHd);
		plVtHd=shl.GetVertexListOfPolygon(plHd);
		if(nPlVt>0 && plVtHd!=NULL)
		{
			int i;
			for(i=0; i<nPlVt; i++)
			{
				YsShellVertexHandle edVt1,edVt2;
				YsShellEdge *edge;

				edVt1=plVtHd[i];
				edVt2=plVtHd[(i+1)%nPlVt];
				if(LookUpEdge(edge,edVt1,edVt2,YSTRUE)==YSOK)
				{
					edge->usedBy.AppendItem(plHd);
				}
				else
				{
					if(AddEdge(edVt1,edVt2,plHd,shl)!=YSOK)
					{
						YsErrOut("YsShellEdgeList::Build()\n");
						YsErrOut("  Failed to add a new edge\n");
					}
				}
			}
		}
	}
	edgeList->Encache();
	return YSOK;
}

YSRESULT YsShellEdgeList::GetEdge(YsShellVertexHandle &edVt1,YsShellVertexHandle &edVt2,int edId) const
{
	YsList <YsShellEdge> *edge;
	edge=edgeList->Seek(edId);
	if(edge!=NULL)
	{
		edVt1=edge->dat.edVt1;
		edVt2=edge->dat.edVt2;
		return YSOK;
	}
	else
	{
		YsErrOut("YsShellEdgeList::GetEdge()\n");
		YsErrOut("  Edge not found\n");
		return YSERR;
	}
}

YSRESULT YsShellEdgeList::LookUpEdge
    (int &edId,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse) const
{
	YsShellEdge *edge;
	if(LookUpEdge(edge,edVt1,edVt2,takeReverse)==YSOK)
	{
		edId=edge->edId;
		return YSOK;
	}
	else
	{
		edId=-1;
		return YSERR;
	}
}

YSRESULT YsShellEdgeList::LookUpEdge
    (YsShellEdge *&edge,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2, YSBOOL takeReverse) const
{
	int i;
	unsigned hashId,edVtKey1,edVtKey2;

	edVtKey1=shell->GetSearchKey(edVt1);
	edVtKey2=shell->GetSearchKey(edVt2);

	hashId=(edVtKey1+edVtKey2)%HashTableSize;
	for(i=0; i<edgeHash[hashId].GetNumItem(); i++)
	{
		YsShellEdge *candidate;
		unsigned canEdVtKey1,canEdVtKey2;

		candidate=edgeHash[hashId].GetItem(i);
		canEdVtKey1=shell->GetSearchKey(candidate->edVt1);
		canEdVtKey2=shell->GetSearchKey(candidate->edVt2);

		if(canEdVtKey1==edVtKey1 && canEdVtKey2==edVtKey2)
		{
			edge=candidate;
			return YSOK;
		}
		else if(takeReverse==YSTRUE && canEdVtKey1==edVtKey2 && canEdVtKey2==edVtKey1)
		{
			edge=candidate;
			return YSOK;
		}
	}

	edge=NULL;
	return YSERR;
}

const YsShellEdge *YsShellEdgeList::GetEdge(int edId) const
{
	YsList <YsShellEdge> *seeker;
	seeker=edgeList->Seek(edId);
	if(seeker!=NULL)
	{
		return &seeker->dat;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellEdgeList::AddEdge
    (YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YsShellPolygonHandle plId,const YsShell &shl)
{
	int edId;
	unsigned edVtKey1,edVtKey2,hashId;
	YsList <YsShellEdge> *edge;

	edge=new YsList <YsShellEdge>;

	if(edge!=NULL)
	{
		edVtKey1=shl.GetSearchKey(edVt1);
		edVtKey2=shl.GetSearchKey(edVt2);

		edId=nEdge;
		hashId=(edVtKey1+edVtKey2)%HashTableSize;

		edge->dat.edId=edId;
		edge->dat.edVt1=edVt1;
		edge->dat.edVt2=edVt2;
		edge->dat.usedBy.AppendItem(plId);
		edgeHash[hashId].AppendItem(&edge->dat);

		nEdge++;

		edgeList=edgeList->Append(edge);

		return YSOK;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::AddEdge()");
		return YSERR;
	}
}


////////////////////////////////////////////////////////////

YsShellVertexStore::YsShellVertexStore()
{
	this->shl=NULL;
	EnableAutoResizing();
}

YsShellVertexStore::YsShellVertexStore(const YsShell &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

YsShellVertexStore::YsShellVertexStore(const YsShell &shl,YSSIZE_T nVtx,const YsShellVertexHandle vtHdArray[])
{
	SetShell(shl);
	EnableAutoResizing();
	AddVertex(nVtx,vtHdArray);
}

YsShellVertexStore::YsShellVertexStore(const YsShell &shl,YsConstArrayMask <YsShellVertexHandle> vtHdArray)
{
	this->shl=&shl;
	EnableAutoResizing();
	Add(vtHdArray.GetN(),vtHdArray);
}

void YsShellVertexStore::CleanUp(void)
{
	YsKeyStore::CleanUp();
}

void YsShellVertexStore::SetShell(const YsShell &shl)
{
	this->shl=&shl;
}

const YsShell &YsShellVertexStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellVertexStore::Add(YsShellVertexHandle vtHd)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		return AddKey(shl->GetSearchKey(vtHd));
	}
}

YSRESULT YsShellVertexStore::AddVertex(YsConstArrayMask <YsShellVertexHandle> vtHdArray)
{
	return Add(vtHdArray.GetN(),vtHdArray);
}

YSRESULT YsShellVertexStore::Add(YsConstArrayMask <YsShellVertexHandle> vtHdArray)
{
	return Add(vtHdArray.GetN(),vtHdArray);
}

YSRESULT YsShellVertexStore::AddVertex(YsShellVertexHandle vtHd)
{
	return Add(vtHd);
}

YSRESULT YsShellVertexStore::Add(YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		int i;
		for(i=0; i<nVt; i++)
		{
			AddKey(shl->GetSearchKey(vtHd[i]));
		}
		return YSOK;
	}
}

YSRESULT YsShellVertexStore::AddVertex(YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	return Add(nVt,vtHd);
}

YSRESULT YsShellVertexStore::Delete(YsShellVertexHandle vtHd)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		return DeleteKey(shl->GetSearchKey(vtHd));
	}
}

YSRESULT YsShellVertexStore::DeleteVertex(YsShellVertexHandle vtHd)
{
	return Delete(vtHd);
}

YSBOOL YsShellVertexStore::IsIncluded(YsShellVertexHandle vtHd) const
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSFALSE;
	}
	else
	{
		return YsKeyStore::IsIncluded(shl->GetSearchKey(vtHd));
	}
}

YsArray <YsShellVertexHandle> YsShellVertexStore::GetArray(void) const
{
	YsArray <YsShellVertexHandle> ary;
	//for(auto vtHd : *this)
	for(auto iter=begin(); end()!=iter; ++iter)
	{
		auto vtHd=*iter;
		ary.Append(vtHd);
	}
	return ary;
}

////////////////////////////////////////////////////////////

YsShellPolygonStore::YsShellPolygonStore()
{
	shl=NULL;
	EnableAutoResizing();
}

YsArray <YsShellPolygonHandle> YsShellPolygonStore::GetArray(void) const
{
	YsArray <YsShellPolygonHandle> ary;
	//for(auto plHd : *this)
	for(auto iter=begin(); iter!=end(); ++iter)
	{
		auto plHd=*iter;
		ary.Append(plHd);
	}
	return ary;
}

YsShellPolygonStore::YsShellPolygonStore(const YsShell &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

YsShellPolygonStore::YsShellPolygonStore(const YsShell &shl,int nPlHd,const YsShellPolygonHandle plHdArray[])
{
	SetShell(shl);
	EnableAutoResizing();
	AddPolygon(nPlHd,plHdArray);
}

YsShellPolygonStore::YsShellPolygonStore(const YsShell &shl,YsConstArrayMask <YsShellPolygonHandle> plHdArray)
{
	this->shl=&shl;
	EnableAutoResizing();
	AddPolygon(plHdArray.GetN(),plHdArray);
}

YSRESULT YsShellPolygonStore::Add(YsConstArrayMask <YsShell::PolygonHandle> plHdArray)
{
	return Add(plHdArray.GetN(),plHdArray);
}

YSRESULT YsShellPolygonStore::AddPolygon(YsConstArrayMask <YsShell::PolygonHandle> plHdArray)
{
	return Add(plHdArray.GetN(),plHdArray);
}

void YsShellPolygonStore::CleanUp(void)
{
	YsKeyStore::CleanUp();
}

void YsShellPolygonStore::SetShell(const YsShell &shl)
{
	this->shl=&shl;
}

const YsShell &YsShellPolygonStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellPolygonStore::Add(YsShellPolygonHandle plHd)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		return AddKey(shl->GetSearchKey(plHd));
	}
}

YSRESULT YsShellPolygonStore::AddPolygon(YsShellPolygonHandle plHd)
{
	return Add(plHd);
}

YSRESULT YsShellPolygonStore::Add(YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		int i;
		for(i=0; i<nPl; i++)
		{
			AddKey(shl->GetSearchKey(plHd[i]));
		}
		return YSOK;
	}
}

YSRESULT YsShellPolygonStore::AddPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHd[])
{
	return Add(nPl,plHd);
}

YSRESULT YsShellPolygonStore::Delete(YsShellPolygonHandle plHd)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		return DeleteKey(shl->GetSearchKey(plHd));
	}
}

YSRESULT YsShellPolygonStore::DeletePolygon(YsShellPolygonHandle plHd)
{
	return Delete(plHd);
}

YSBOOL YsShellPolygonStore::IsIncluded(YsShellPolygonHandle plHd) const
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSFALSE;
	}
	else
	{
		return YsKeyStore::IsIncluded(shl->GetSearchKey(plHd));
	}
}

////////////////////////////////////////////////////////////

YsShellEdgeStore::YsShellEdgeStore()
{
	shl=NULL;
	EnableAutoResizing();
}

YsShellEdgeStore::YsShellEdgeStore(const YsShell &shl)
{
	SetShell(shl);
	EnableAutoResizing();
}

void YsShellEdgeStore::CleanUp(void)
{
	YsTwoKeyStore::CleanUp();
}

void YsShellEdgeStore::SetShell(const YsShell &shl)
{
	this->shl=&shl;
}

const YsShell &YsShellEdgeStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellEdgeStore::Add(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd1);
		key2=shl->GetSearchKey(edVtHd2);
		return AddKey(key1,key2);
	}
}

YSRESULT YsShellEdgeStore::AddEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	return Add(edVtHd1,edVtHd2);
}

YSRESULT YsShellEdgeStore::Add(const YsShellVertexHandle edVtHd[2])
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd[0]);
		key2=shl->GetSearchKey(edVtHd[1]);
		return AddKey(key1,key2);
	}
}

YSRESULT YsShellEdgeStore::AddEdge(const YsShellVertexHandle edVtHd[2])
{
	return Add(edVtHd);
}

YSRESULT YsShellEdgeStore::Delete(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd1);
		key2=shl->GetSearchKey(edVtHd2);
		return DeleteKey(key1,key2);
	}
}

YSRESULT YsShellEdgeStore::DeleteEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	return Delete(edVtHd1,edVtHd2);
}

YSRESULT YsShellEdgeStore::Delete(const YsShellVertexHandle edVtHd[2])
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSERR;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd[0]);
		key2=shl->GetSearchKey(edVtHd[1]);
		return DeleteKey(key1,key2);
	}
}

YSRESULT YsShellEdgeStore::DeleteEdge(const YsShellVertexHandle edVtHd[2])
{
	return Delete(edVtHd);
}

YSBOOL YsShellEdgeStore::IsIncluded(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSFALSE;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd1);
		key2=shl->GetSearchKey(edVtHd2);
		return YsTwoKeyStore::IsIncluded(key1,key2);
	}
}

YSBOOL YsShellEdgeStore::IsIncluded(const YsShellVertexHandle edVtHd[2]) const
{
	if(NULL==shl)
	{
		YsErrOut("%s\n",__FUNCTION__);
		YsErrOut("  YsShell hasn't been set!\n");
		return YSFALSE;
	}
	else
	{
		YSHASHKEY key1,key2;
		key1=shl->GetSearchKey(edVtHd[0]);
		key2=shl->GetSearchKey(edVtHd[1]);
		return YsTwoKeyStore::IsIncluded(key1,key2);
	}
}

////////////////////////////////////////////////////////////

static YSBOOL YsSharedPolygonIsBoundary
    (YsShell &newShell,int sharedPlId,
     const YsShell &sh1,int fromPlId1,
     const YsShell &sh2,int fromPlId2)
{
	int nVtxOfPlg,crsCtr1,crsCtr2;
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

			side1=sh1.CountRayIntersection(crsCtr1,inside,ray,fromPlId1);
			side2=sh2.CountRayIntersection(crsCtr2,inside,ray,fromPlId2);

			if(side1!=YSBOUNDARY && side1!=YSUNKNOWNSIDE &&
			   side2!=YSBOUNDARY && side2!=YSUNKNOWNSIDE)
			{
				if((crsCtr1%2)==(crsCtr2%2))
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

	YsErrOut("YsPolygonIsInside()@ysshell.cpp\n  Overlapping polygon may exists.\n");
	return YSTRUE;
}

YSRESULT YsBlendAnd
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);

		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)!=YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}
	newShell.DeleteVertexAtTheSamePosition();
	newShell.OmitPolygonAfterBlend();

	return YSOK;
}





YSRESULT YsBlendOr
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)==YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}

	newShell.DeleteVertexAtTheSamePosition();

	printf("####newShell.OmitPolygonAfterBlend() is temporarily disabled in YsBlendOr()@ysshell.cpp\n");
	//newShell.OmitPolygonAfterBlend();

	return YSOK;
}

YSRESULT YsBlendMinus
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)==YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
		else if(plg->freeAttribute2==1)
		{
			newShell.InvertPolygon(i);
		}
	}

	newShell.DeleteVertexAtTheSamePosition();
	newShell.OmitPolygonAfterBlend();

	return YSOK;
}

////////////////////////////////////////////////////////////

extern YSRESULT YsMixShell(YsShell &newShell,const YsShell &sh1,const YsShell &sh2);

YSRESULT YsBlendShell
   (YsShell &newShell,const YsShell &sh1,const YsShell &sh2,
    YSBOOLEANOPERATION boolop)
    // Note:This function will use freeAttribute2 and 3.
{
	if(YSBOOLSEPARATE==boolop)
	{
		// Must use YsBlendShell3
		return YSERR;
	}

	newShell.CleanUp();
	sh1.Encache();
	sh2.Encache();

	if(YsMixShell(newShell,sh1,sh2)==YSOK)
	{
		switch(boolop)
		{
		case YSBOOLBLEND:
			// It's done.
			return YSOK;
		case YSBOOLAND:
			return YsBlendAnd(newShell,sh1,sh2);
		case YSBOOLOR:
			return YsBlendOr(newShell,sh1,sh2);
		case YSBOOLMINUS:
			return YsBlendMinus(newShell,sh1,sh2);
		default:
			// Unsupported
			return YSERR;
		}
		return YSOK;
	}

	return YSERR;
}


YSRESULT YsSeparateShell
    (YsShell &sh1,YsShell &sh2,const YsShell &org,const YsShell &ref)
{
	sh1.CleanUp();
	sh2.CleanUp();

	if(YsBlendShell1to2(sh1,org,ref,0,YSFALSE)==YSOK &&
	   YsBlendShell1to2(sh1,ref,org,1,YSTRUE)==YSOK)
	{
		sh2=sh1;
		if(YsBlendMinus(sh1,org,ref)==YSOK &&
		   YsBlendAnd(sh2,org,ref)==YSOK)
		{
			return YSOK;
		}
	}
	return YSERR;
}


////////////////////////////////////////////////////////////


YSRESULT YsReplaceShellEdge
    (YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,YSSIZE_T nNewEdVt,const YsShellVertexHandle newEdVt[])
{
	const YsShellSearchTable *search;
	search=shl.GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsReplaceShellEdge()\n");
		YsErrOut("  This function requires search table.\n");
		return YSERR;
	}


	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	if(search->FindPolygonListByEdge(nFoundPlHd,foundPlHd,shl,vtHd1,vtHd2)==YSOK && nFoundPlHd>0)
	{
		int i;
		YSBOOL inserted;
		inserted=YSFALSE;

		YsArray <YsShellPolygonHandle,2> plHdLst;
		plHdLst.Set(nFoundPlHd,foundPlHd);

		for(i=0; i<plHdLst.GetNumItem(); i++)
		{
			YsShellPolygonHandle plHd;
			int nOrgPlVt;
			const YsShellVertexHandle *orgVtHdLst;
			YsArray <YsShellVertexHandle,16> neoVtHdLst;

			plHd=plHdLst.GetItem(i);
			nOrgPlVt=shl.GetNumVertexOfPolygon(plHd);
			orgVtHdLst=shl.GetVertexListOfPolygon(plHd);

			int j;
			neoVtHdLst.Set(0,NULL);
			for(j=0; j<nOrgPlVt; j++)
			{
				if(orgVtHdLst[j]!=vtHd1 && orgVtHdLst[j]!=vtHd2)
				{
					neoVtHdLst.AppendItem(orgVtHdLst[j]);
				}
				if(orgVtHdLst[j]==vtHd1 && orgVtHdLst[(j+1)%nOrgPlVt]==vtHd2)
				{
					YSSIZE_T k;
					for(k=0; k<nNewEdVt; k++)
					{
						neoVtHdLst.AppendItem(newEdVt[k]);
					}
					inserted=YSTRUE;
				}
				else if(orgVtHdLst[j]==vtHd2 && orgVtHdLst[(j+1)%nOrgPlVt]==vtHd1)
				{
					YSSIZE_T k;
					for(k=nNewEdVt-1; k>=0; k--)
					{
						neoVtHdLst.AppendItem(newEdVt[k]);
					}
					inserted=YSTRUE;
				}
			}

			if(inserted==YSTRUE)
			{
				shl.ModifyPolygon(plHd,neoVtHdLst.GetNumItem(),neoVtHdLst.GetArray());
			}
			else
			{
				YsErrOut("YsReplaceShellEdge()\n");
				YsErrOut("  The search table is broken.\n");
				return YSERR;
			}
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


////////////////////////////////////////////////////////////


YSRESULT YsAddShell(YsShell &shl1,const YsShell &shl2)
{
	YsHashTable <YsShellVertexHandle> findVtx(1024);
	YsShellVertexHandle vtHd,newVtHd;
	unsigned key;

	vtHd=NULL;
	while((vtHd=shl2.FindNextVertex(vtHd))!=NULL)
	{
		YsVec3 pos;
		shl2.GetVertexPosition(pos,vtHd);
		newVtHd=shl1.AddVertexH(pos);
		key=shl2.GetSearchKey(vtHd);

		findVtx.AddElement(key,newVtHd);
	}

	int i,nPlVt;
	YsShellPolygonHandle plHd;
	const YsShellVertexHandle *plVtHd;
	YsArray <YsShellVertexHandle> newPlVtHd;

	plHd=NULL;
	while((plHd=shl2.FindNextPolygon(plHd))!=NULL)
	{
		nPlVt=shl2.GetNumVertexOfPolygon(plHd);
		plVtHd=shl2.GetVertexListOfPolygon(plHd);

		newPlVtHd.Set(0,NULL);
		for(i=0; i<nPlVt; i++)
		{
			key=shl2.GetSearchKey(plVtHd[i]);
			if(findVtx.FindElement(vtHd,key)==YSOK)
			{
				newPlVtHd.AppendItem(vtHd);
			}
			else
			{
				return YSERR;
			}
		}
		shl1.AddPolygonH(nPlVt,newPlVtHd.GetArray());
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

YsShellPassThroughSource::YsShellPassThroughSource(const YsShell &shl)
{
	shlPtr=&shl;
}
/* virtual */ YsVec3 YsShellPassThroughSource::GetVertexPosition(YsEditArrayObjectHandle <YsShellVertex> vtHd) const
{
	return shlPtr->GetVertexPosition(vtHd);
}

