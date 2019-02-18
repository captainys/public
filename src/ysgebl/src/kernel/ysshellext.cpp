/* ////////////////////////////////////////////////////////////

File Name: ysshellext.cpp
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

#include <ysvisited.h>

#include "ysshellext.h"
#include "ysshellextmisc.h"
#include "ysshellextio.h"
#include "ysshellextsearch.h"


void YsShellExt::CommonProp::Initialize(void)
{
	ident=-1;
	label.Set("");
	refCount=0;
	refCountFromFrozenEntity=0;
}

YsShellExt::BoundaryLayerSpec::BoundaryLayerSpec()
{
	Initialize();
}
void YsShellExt::BoundaryLayerSpec::Initialize(void)
{
	nLayer=0;
	firstLayerThickness=0.0;
	growthFuncType=0;
	growthFuncParam=1.0;
}
YsArray <double> YsShellExt::BoundaryLayerSpec::CalculateLayerThickness(void) const
{
	YsArray <double> thickness(nLayer,nullptr);
	switch(growthFuncType)
	{
	case 0:
	default:
		for(auto &t : thickness)
		{
			t=firstLayerThickness;
		}
		break;
	case 1:
		{
			double y=firstLayerThickness;
			for(auto &t : thickness)
			{
				t=y;
				y*=growthFuncParam;
			}
		}
		break;
	}
	return thickness;
}

void YsShellExt::ConstEdgeCoreAttrib::Initialize(void)
{
	flags=ZEROFLAG;
	bLayerSpec.Initialize();
	elemSize=0.0;
}

YSBOOL YsShellExt::ConstEdgeCoreAttrib::IsCrease(void) const
{
	return ((flags & CREASE) ? YSTRUE : YSFALSE);
}
void YsShellExt::ConstEdgeCoreAttrib::SetIsCrease(YSBOOL isCrease)
{
	if(YSTRUE==isCrease)
	{
		flags=(CEFLAG)(flags|CREASE);
	}
	else
	{
		flags=(CEFLAG)(flags&~CREASE);
	}
}

void YsShellExt::ConstEdgeCoreAttrib::SetNoBubble(YSBOOL noBubble)
{
	if(YSTRUE==noBubble)
	{
		flags=(CEFLAG)(flags|NOBUBBLE);
	}
	else
	{
		flags=(CEFLAG)(flags&~NOBUBBLE);
	}
}
YSBOOL YsShellExt::ConstEdgeCoreAttrib::NoBubble(void) const
{
	return ((flags & NOBUBBLE) ? YSTRUE : YSFALSE);
}

void YsShellExt::ConstEdgeCoreAttrib::SetNoRemesh(YSBOOL noRemesh)
{
	if(YSTRUE==noRemesh)
	{
		flags=(CEFLAG)(flags|NOREMESH);
	}
	else
	{
		flags=(CEFLAG)(flags&~NOREMESH);
	}
}

YSBOOL YsShellExt::ConstEdgeCoreAttrib::NoRemesh(void) const
{
	return ((flags & NOREMESH) ? YSTRUE : YSFALSE);
}

void YsShellExt::ConstEdgeCoreAttrib::SetIsTemporary(YSBOOL flag)
{
	if(YSTRUE==flag)
	{
		flags=(CEFLAG)(flags|ISTEMPORARY);
	}
	else
	{
		flags=(CEFLAG)(flags&~ISTEMPORARY);
	}
}
YSBOOL YsShellExt::ConstEdgeCoreAttrib::IsTemporary(void) const
{
	return ((flags & ISTEMPORARY) ? YSTRUE : YSFALSE);
}


void YsShellExt::ConstEdge::Initialize(void)
{
	ConstEdgeAttrib::Initialize();
	vtHdArray.Clear();
	isLoop=YSFALSE;
}

YSBOOL YsShellExt::ConstEdge::IsLoop(void) const
{
	return isLoop;
}

void YsShellExt::ConstEdge::SetIsLoop(YSBOOL isLoop)
{
	this->isLoop=isLoop;
}


void YsShellExt::FaceGroupCoreAttrib::Initialize(void)
{
	flags=ZEROFLAG;
	bLayerSpec.Initialize();
	elemSize=0.0;
}

void YsShellExt::FaceGroupCoreAttrib::SetNoBubble(YSBOOL noBubble)
{
	if(YSTRUE==noBubble)
	{
		flags=(FGFLAG)(flags|NOBUBBLE);
	}
	else
	{
		flags=(FGFLAG)(flags&~NOBUBBLE);
	}
}
YSBOOL YsShellExt::FaceGroupCoreAttrib::NoBubble(void) const
{
	return ((flags & NOBUBBLE) ? YSTRUE : YSFALSE);
}

void YsShellExt::FaceGroupCoreAttrib::SetNoRemesh(YSBOOL noRemesh)
{
	if(YSTRUE==noRemesh)
	{
		flags=(FGFLAG)(flags|NOREMESH);
	}
	else
	{
		flags=(FGFLAG)(flags&~NOREMESH);
	}
}
YSBOOL YsShellExt::FaceGroupCoreAttrib::NoRemesh(void) const
{
	return ((flags & NOREMESH) ? YSTRUE : YSFALSE);
}

void YsShellExt::FaceGroupCoreAttrib::SetInteriorConstSurfaceFlag(YSBOOL flag)
{
	if(YSTRUE==flag)
	{
		flags=(FGFLAG)(flags|INTERIOR_CONST_SURFACE);
	}
	else
	{
		flags=(FGFLAG)(flags&~INTERIOR_CONST_SURFACE);
	}
}
YSBOOL YsShellExt::FaceGroupCoreAttrib::InteriorConstSurfaceFlag(void) const
{
	return ((flags & INTERIOR_CONST_SURFACE) ? YSTRUE : YSFALSE);
}
void YsShellExt::FaceGroupCoreAttrib::SetIsTemporary(YSBOOL flag)
{
	if(YSTRUE==flag)
	{
		flags=(FGFLAG)(flags|ISTEMPORARY);
	}
	else
	{
		flags=(FGFLAG)(flags&~ISTEMPORARY);
	}
}
YSBOOL YsShellExt::FaceGroupCoreAttrib::IsTemporary(void) const
{
	return ((flags & ISTEMPORARY) ? YSTRUE : YSFALSE);
}

void YsShellExt::FaceGroup::Initialize(void)
{
	FaceGroupCoreAttrib::Initialize();
	fgPlHd.CleanUp();
}




void YsShellExt::Volume::Initialize(void)
{
	fgHdArray.Clear();
}



void YsShellExt::VertexAttrib::Initialize(void)
{
	flags=ZEROFLAG;
	refCount=0;
	refCountFromFrozenEntity=0;
	thickness=0.0;
	oPlId=-1;
	oVtId[0]=-1;
	oVtId[1]=-1;
}

YSBOOL YsShellExt::VertexAttrib::IsRound(void) const
{
	return (0!=(flags&ROUND) ? YSTRUE : YSFALSE);
}

void YsShellExt::VertexAttrib::SetRound(YSBOOL round)
{
	if(YSTRUE==round)
	{
		flags=(VTFLAG)(flags|ROUND);
	}
	else
	{
		flags=(VTFLAG)(flags&~ROUND);
	}
}

YSBOOL YsShellExt::VertexAttrib::IsConstrained(void) const
{
	return (0!=(flags&CONSTRAINED) ? YSTRUE : YSFALSE);
}

void YsShellExt::VertexAttrib::SetConstrained(YSBOOL isConstrained)
{
	if(YSTRUE==isConstrained)
	{
		flags=(VTFLAG)(flags|CONSTRAINED);
	}
	else
	{
		flags=(VTFLAG)(flags&~CONSTRAINED);
	}
}

void YsShellExt::VertexAttrib::SetVertexTrackingToVertex(int oVtId)
{
	this->oVtId[0]=oVtId;
	this->oVtId[1]=oVtId;
}

void YsShellExt::VertexAttrib::SetVertexTrackingToEdge(int oEdVtId0,int oEdVtId1)
{
	this->oVtId[0]=oEdVtId0;
	this->oVtId[1]=oEdVtId1;
}

void YsShellExt::VertexAttrib::SetVertexTrackingToPolygon(int oPlId)
{
	this->oPlId=oPlId;
}

void YsShellExt::VertexAttrib::CopyFrom(const VertexAttrib &incoming)
{
	flags=incoming.flags;
	thickness=incoming.thickness;
	oPlId=incoming.oPlId;
	oVtId[0]=incoming.oVtId[0];
	oVtId[1]=incoming.oVtId[1];
}

void YsShellExt::PolygonAttrib::Initialize(void)
{
	flags=ZEROFLAG;
	refCount=0;
	refCountFromFrozenEntity=0;
	elemSize=0.0;
	thickness=0.0;
}

YSBOOL YsShellExt::PolygonAttrib::GetNoShading(void) const
{
	return (0!=(flags&NO_SHADING) ? YSTRUE : YSFALSE);
}

void YsShellExt::PolygonAttrib::SetNoShading(YSBOOL noShading)
{
	if(YSTRUE==noShading)
	{
		flags=(PLFLAG)(flags|NO_SHADING);
	}
	else
	{
		flags=(PLFLAG)(flags&~NO_SHADING);
	}
}

void YsShellExt::PolygonAttrib::SetTwoSided(YSBOOL twoSided)
{
	if(YSTRUE==twoSided)
	{
		flags=(PLFLAG)(flags|TWO_SIDED);
	}
	else
	{
		flags=(PLFLAG)(flags&~TWO_SIDED);
	}
}

YSBOOL YsShellExt::PolygonAttrib::GetAsLight(void) const
{
	return (0!=(flags&AS_LIGHT) ? YSTRUE : YSFALSE);
}
void YsShellExt::PolygonAttrib::SetAsLight(YSBOOL asLight)
{
	if(YSTRUE==asLight)
	{
		flags=(PLFLAG)(flags|AS_LIGHT);
	}
	else
	{
		flags=(PLFLAG)(flags&~AS_LIGHT);
	}
}

void YsShellExt::PolygonAttrib::CopyFrom(const PolygonAttrib &incoming) // Copy except reference count
{
	flags=incoming.flags;
	elemSize=incoming.elemSize;
	thickness=incoming.thickness;
}



////////////////////////////////////////////////////////////



YsShellExt::YsShellExt()
{
	constEdgeArray.EnableSearch();
	faceGroupArray.EnableSearch();
	volumeArray.EnableSearch();

	shlSearch=NULL;
	extSearch=NULL;

	drawingBuffer=new YsShellExtDrawingBuffer;
}

YsShellExt::~YsShellExt()
{
	delete drawingBuffer;
	DisableSearch();
}

const YsShellExt &YsShellExt::operator=(const YsShellExt &from)
{
	CopyFrom(from);
	return *this;
}

const YsShellExt &YsShellExt::operator=(const YsShell &from)
{
	CopyFrom(from);
	return *this;
}

YsShellExt::YsShellExt(const YsShellExt &from)
{
	constEdgeArray.EnableSearch();
	faceGroupArray.EnableSearch();
	volumeArray.EnableSearch();

	shlSearch=NULL;
	extSearch=NULL;

	drawingBuffer=new YsShellExtDrawingBuffer;

	CopyFrom(from);
}

void YsShellExt::CopyFrom(const YsShellExt &from)
{
	if(this==&from)
	{
		return;
	}

	CleanUp();

	YsHashTable <YsShellVertexHandle> fromVtKeyToNewVtHd;
	for(auto fromVtHd : from.AllVertex())
	{
		auto newVtHd=AddVertex(from.GetVertexPosition(fromVtHd));
		auto attrib=from.GetVertexAttrib(fromVtHd);
		SetVertexAttrib(newVtHd,*attrib);

		fromVtKeyToNewVtHd.AddElement(from.GetSearchKey(fromVtHd),newVtHd);
	}

	YsHashTable <YsShellPolygonHandle> fromPlKeyToNewPlHd;
	for(auto fromPlHd : from.AllPolygon())
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		from.GetPolygon(plVtHd,fromPlHd);

		YSBOOL skip=YSFALSE;
		for(auto &vtHd : plVtHd)
		{
			if(YSOK!=fromVtKeyToNewVtHd.FindElement(vtHd,from.GetSearchKey(vtHd)))
			{
				skip=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=skip)
		{
			auto newPlHd=AddPolygon(plVtHd);
			auto attrib=from.GetPolygonAttrib(fromPlHd);
			auto nom=from.GetNormal(fromPlHd);
			auto col=from.GetColor(fromPlHd);

			SetPolygonAttrib(newPlHd,*attrib);
			SetPolygonNormal(newPlHd,nom);
			SetPolygonColor(newPlHd,col);

			fromPlKeyToNewPlHd.AddElement(from.GetSearchKey(fromPlHd),newPlHd);
		}
	}

	for(auto fromCeHd : from.AllConstEdge())
	{
		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		from.GetConstEdge(ceVtHd,isLoop,fromCeHd);

		YSBOOL skip=YSFALSE;
		for(auto &vtHd : ceVtHd)
		{
			if(YSOK!=fromVtKeyToNewVtHd.FindElement(vtHd,from.GetSearchKey(vtHd)))
			{
				skip=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=skip)
		{
			auto newCeHd=AddConstEdge(ceVtHd,isLoop);
			auto attrib=from.GetConstEdgeAttrib(fromCeHd);
			SetConstEdgeAttrib(newCeHd,attrib);
		}
	}

	YsHashTable <YsShellExt::FaceGroupHandle> fromFgKeyToNewFgHd;
	for(auto fromFgHd : from.AllFaceGroup())
	{
		YsArray <YsShellPolygonHandle> fgPlHd;
		from.GetFaceGroup(fgPlHd,fromFgHd);

		YSBOOL skip=YSFALSE;
		for(auto &plHd : fgPlHd)
		{
			if(YSOK!=fromPlKeyToNewPlHd.FindElement(plHd,from.GetSearchKey(plHd)))
			{
				skip=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=skip)
		{
			auto newFgHd=AddFaceGroup(fgPlHd);
			auto attrib=from.GetFaceGroupAttrib(fromFgHd);
			SetFaceGroupAttrib(newFgHd,attrib);

			fromFgKeyToNewFgHd.AddElement(from.GetSearchKey(fromFgHd),newFgHd);
		}
	}

	for(auto fromVlHd : from.AllVolume())
	{
		YsArray <YsShellExt::FaceGroupHandle> vlFgHd;
		from.GetVolume(vlFgHd,fromVlHd);

		YSBOOL skip=YSFALSE;
		for(auto &fgHd : vlFgHd)
		{
			if(YSOK!=fromFgKeyToNewFgHd.FindElement(fgHd,from.GetSearchKey(fgHd)))
			{
				skip=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=skip)
		{
			auto newVlHd=AddVolume(vlFgHd);
			auto attrib=from.GetVolumeAttrib(fromVlHd);
			SetVolumeAttrib(newVlHd,attrib);
		}
	}

	YsHasTextMetaData::CopyFrom(from);
}

void YsShellExt::CopyFrom(const YsShell &from)
{
	if(this==&from)
	{
		return;
	}

	CleanUp();
	YsShell::CopyFrom(from);

	for(auto vtHd : AllVertex())
	{
		YsEditArrayObjectHandle <VertexAttrib> vtAttribHd=vtAttribArray.Create();
		vtKeyToVtAttribArrayHandle.AddElement(GetSearchKey(vtHd),vtAttribHd);

		vtAttribArray[vtAttribHd]->Initialize();
	}

	for(auto plHd : AllPolygon())
	{
		YsEditArrayObjectHandle <PolygonAttrib> plAttribHd=plAttribArray.Create();
		plKeyToPlAttribArrayHandle.AddElement(GetSearchKey(plHd),plAttribHd);

		plAttribArray[plAttribHd]->Initialize();

		for(auto vtHd : GetPolygonVertex(plHd))
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
			++(vtAttrib->refCount);
		}
	}
}

void YsShellExt::MoveFrom(YsShellExt &from)
{
	if(this==&from)
	{
		return;
	}

	CleanUp();

	shlSearch=from.shlSearch;
	extSearch=from.extSearch;
	from.shlSearch=nullptr;
	from.extSearch=nullptr;

	this->matrixIsSet=from.matrixIsSet;
	this->mat=from.mat;

	this->vtKeyToVtAttribArrayHandle.MoveFrom(from.vtKeyToVtAttribArrayHandle);
	this->vtAttribArray.MoveFrom(from.vtAttribArray);

	this->plKeyToPlAttribArrayHandle.MoveFrom(from.plKeyToPlAttribArrayHandle);
	this->plAttribArray.MoveFrom(from.plAttribArray);

	this->vtx.MoveFrom(from.vtx);
	this->plg.MoveFrom(from.plg);
	YsHasTextMetaData::MoveFrom(from);
	this->constEdgeArray.MoveFrom(from.constEdgeArray);
	this->faceGroupArray.MoveFrom(from.faceGroupArray);
	this->volumeArray.MoveFrom(from.volumeArray);
}

void YsShellExt::CleanUp(void)
{
	YsShell::CleanUp();

	vtKeyToVtAttribArrayHandle.CleanUp();
	vtAttribArray.CleanUp();

	plKeyToPlAttribArrayHandle.CleanUp();
	plAttribArray.CleanUp();

	constEdgeArray.CleanUp();
	faceGroupArray.CleanUp();
	volumeArray.CleanUp();
}

void YsShellExt::CacheVertexNormal(void)
{
	AutoComputeVertexNormalAll(YSTRUE);
}

void YsShellExt::Encache(void) const
{
	YsShell::Encache();
	constEdgeArray.Encache();
	faceGroupArray.Encache();
	volumeArray.Encache();
}

YSRESULT YsShellExt::LoadSrf(YsTextInputStream &inStream)
{
	YsShellExtReader srfReader;
	return srfReader.MergeSrf(*this,inStream);
}

YSRESULT YsShellExt::LoadStl(const char fn[])
{
	YSRESULT res=YsShell::LoadStl(fn);
	if(YSOK==res)
	{
		for(YsShellVertexHandle vtHd=NULL; YSOK==MoveToNextVertex(vtHd); )
		{
			const YSHASHKEY vtKey=GetSearchKey(vtHd);

			YsEditArrayObjectHandle <VertexAttrib> vtAttribHd=vtAttribArray.Create();
			vtKeyToVtAttribArrayHandle.AddElement(vtKey,vtAttribHd);

			vtAttribArray[vtAttribHd]->Initialize();
		}

		for(YsShellPolygonHandle plHd=NULL; YSOK==MoveToNextPolygon(plHd); )
		{
			const YSHASHKEY plKey=GetSearchKey(plHd);

			YsEditArrayObjectHandle <PolygonAttrib> plAttribHd=plAttribArray.Create();
			plKeyToPlAttribArrayHandle.AddElement(plKey,plAttribHd);

			plAttribArray[plAttribHd]->Initialize();

			int nPlVt;
			const YsShellVertexHandle *plVtHd;
			GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

			for(YSSIZE_T idx=0; idx<nPlVt; ++idx)
			{
				VertexAttrib *vtAttrib=GetVertexAttrib(plVtHd[idx]);
				++(vtAttrib->refCount);
			}
		}
	}
	return res;
}

YSRESULT YsShellExt::LoadObj(YsTextInputStream &inStream)
{
	YsShellExtObjReader::ReadOption opt;
	YsShellExtObjReader objReader;
	return objReader.ReadObj(*this,inStream,opt);
}

YsShellExtDrawingBuffer &YsShellExt::GetDrawingBuffer(void) const
{
	return *drawingBuffer;
}

YsShellVertexHandle YsShellExt::AddVertex(const YsVec3 &pos)
{
	const YsShellVertexHandle newVtHd=YsShell::AddVertexH(pos);
	const YSHASHKEY vtKey=GetSearchKey(newVtHd);

	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd=vtAttribArray.Create();
	vtKeyToVtAttribArrayHandle.AddElement(vtKey,vtAttribHd);

	vtAttribArray[vtAttribHd]->Initialize();

	return newVtHd;
}

YSRESULT YsShellExt::SetVertexPosition(VertexHandle vtHd,const YsVec3 &pos)
{
	return ModifyVertexPosition(vtHd,pos);
}

YSRESULT YsShellExt::SetVertexRoundFlag(YsShellVertexHandle vtHd,YSBOOL round)
{
	VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib)
	{
		vtAttrib->SetRound(round);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::MoveToNextVertex(YsShellVertexHandle &vtHd) const
{
	vtHd=FindNextVertex(vtHd);
	if(NULL!=vtHd)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellExt::MoveToNextPolygon(YsShellPolygonHandle &plHd) const
{
	plHd=FindNextPolygon(plHd);
	if(NULL!=plHd)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellExt::SetVertexConstrainedFlag(YsShellVertexHandle vtHd,YSBOOL round)
{
	VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib)
	{
		vtAttrib->SetConstrained(round);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::FreezeVertex(YsShellVertexHandle vtHd)
{
	VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib && 0==vtAttrib->refCount)
	{
		return YsShell::FreezeVertex(vtHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::MeltVertex(YsShellVertexHandle vtHd)
{
	return YsShell::MeltVertex(vtHd);
}

YSRESULT YsShellExt::DeleteVertex(YsShellVertexHandle vtHd)
{
	VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib && YSTRUE!=IsFrozen(vtHd) && 0==vtAttrib->refCount && 0==vtAttrib->refCountFromFrozenEntity)
	{
		return YsShell::DeleteVertex(vtHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteFrozenVertex(YsShellVertexHandle vtHd)
{
	YsShellExt::VertexAttrib *attrib=GetVertexAttribMaybeFrozen(vtHd);
	if(NULL!=attrib && YSTRUE==IsFrozen(vtHd) && 0==attrib->refCount && 0==attrib->refCountFromFrozenEntity)
	{
		YsShell::DeleteFrozenVertex(vtHd);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetVertexTrackingToVertex(YsShellVertexHandle vtHd,int oVtId)
{
	YsShellExt::VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib)
	{
		vtAttrib->SetVertexTrackingToVertex(oVtId);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetVertexTrackingToEdge(YsShellVertexHandle vtHd,int oEdVtId0,int oEdVtId1)
{
	YsShellExt::VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib)
	{
		vtAttrib->SetVertexTrackingToEdge(oEdVtId0,oEdVtId1);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetVertexTrackingToPolygon(YsShellVertexHandle vtHd,int oPlId)
{
	YsShellExt::VertexAttrib *vtAttrib=GetVertexAttrib(vtHd);
	if(NULL!=vtAttrib)
	{
		vtAttrib->SetVertexTrackingToPolygon(oPlId);
		return YSOK;
	}
	return YSERR;
}

YsShell::PolygonHandle YsShellExt::AddPolygon(YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	const YsShellPolygonHandle newPlHd=YsShell::AddPolygonH(nVt,vtHdArray);
	const YSHASHKEY plKey=GetSearchKey(newPlHd);

	YsEditArrayObjectHandle <PolygonAttrib> plAttribHd=plAttribArray.Create();
	plKeyToPlAttribArrayHandle.AddElement(plKey,plAttribHd);

	plAttribArray[plAttribHd]->Initialize();

	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		VertexAttrib *vtAttrib=GetVertexAttrib(vtHdArray[idx]);
		++(vtAttrib->refCount);
	}

	return newPlHd;
}

YsShell::PolygonHandle YsShellExt::AddPolygon(const YsConstArrayMask <YsShellVertexHandle> &vtHdArray)
{
	return AddPolygon(vtHdArray.GetN(),vtHdArray);
}

YSRESULT YsShellExt::SetPolygonVertex(YsShellPolygonHandle plHd,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	YsArray <YsShellVertexHandle,16> oldPlVtHd;
	GetVertexListOfPolygon(oldPlVtHd,plHd);
	for(YSSIZE_T idx=0; idx<oldPlVtHd.GetN(); ++idx)
	{
		VertexAttrib *vtAttrib=GetVertexAttrib(oldPlVtHd[idx]);
		--(vtAttrib->refCount);
	}

	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		VertexAttrib *vtAttrib=GetVertexAttrib(vtHdArray[idx]);
		++(vtAttrib->refCount);
	}

	return YsShell::ModifyPolygon(plHd,nVt,vtHdArray);
}

YSRESULT YsShellExt::SetPolygonVertex(YsShellPolygonHandle plHd,const YsConstArrayMask <YsShellVertexHandle> &vtHdArray)
{
	return SetPolygonVertex(plHd,vtHdArray.GetN(),vtHdArray);
}

YSRESULT YsShellExt::SetPolygonNoShadingFlag(YsShellPolygonHandle plHd,YSBOOL noShading)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib)
	{
		plAttrib->SetNoShading(noShading);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetPolygonAsLightFlag(YsShellPolygonHandle plHd,YSBOOL asLight)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib)
	{
		plAttrib->SetAsLight(asLight);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetPolygonTwoSidedFlag(YsShellPolygonHandle plHd,YSBOOL twoSided)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib)
	{
		plAttrib->SetTwoSided(twoSided);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetPolygonDesiredElementSize(YsShellPolygonHandle plHd,const double elemSize)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib)
	{
		plAttrib->elemSize=elemSize;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetPolygonColor(YsShellPolygonHandle plHd,const YsColor &col)
{
	return YsShell::SetColorOfPolygon(plHd,col);
}

YSRESULT YsShellExt::SetPolygonNormal(YsShellPolygonHandle plHd,const YsVec3 &nom)
{
	return YsShell::SetNormalOfPolygon(plHd,nom);
}

YSRESULT YsShellExt::SetPolygonAlpha(YsShellPolygonHandle plHd,const double alpha)
{
	if(NULL!=plHd)
	{
		YsColor col;
		GetColorOfPolygon(col,plHd);
		col.SetAd(alpha);
		SetColorOfPolygon(plHd,col);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::FreezePolygon(YsShellPolygonHandle plHd)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib && YSTRUE!=YsShell::IsFrozen(plHd) && 0==plAttrib->refCount)
	{
		int nVt;
		const YsShellVertexHandle *plVtHd;
		GetVertexListOfPolygon(nVt,plVtHd,plHd);

		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(plVtHd[idx]);
			--vtAttrib->refCount;
			++vtAttrib->refCountFromFrozenEntity;
		}

		return YsShell::FreezePolygon(plHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::MeltPolygon(YsShellPolygonHandle plHd)
{
	if(YSTRUE==YsShell::IsFrozen(plHd))
	{
		YsShell::MeltPolygon(plHd);

		int nVt;
		const YsShellVertexHandle *plVtHd;
		GetVertexListOfPolygon(nVt,plVtHd,plHd);

		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(plVtHd[idx]);
			++vtAttrib->refCount;
			--vtAttrib->refCountFromFrozenEntity;
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeletePolygon(YsShellPolygonHandle plHd)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib && YSTRUE!=IsFrozen(plHd) && 0==plAttrib->refCount && 0==plAttrib->refCountFromFrozenEntity)
	{
		int nVt;
		const YsShellVertexHandle *plVtHd;
		GetVertexListOfPolygon(nVt,plVtHd,plHd);

		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(plVtHd[idx]);
			--vtAttrib->refCount;
		}

		return YsShell::DeletePolygon(plHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::ForceDeletePolygon(YsShellPolygonHandle plHd)
{
	DeleteFaceGroupPolygon(plHd);
	DeletePolygon(plHd);
	return YSOK;
}

YSRESULT YsShellExt::ForceDeleteMultiPolygon(YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		DeleteFaceGroupPolygon(plHdArray[idx]);
		DeletePolygon(plHdArray[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExt::ForceDeleteMultiPolygon(const YsConstArrayMask <YsShellPolygonHandle> &plHdArray)
{
	return ForceDeleteMultiPolygon(plHdArray.GetN(),plHdArray);
}

YSRESULT YsShellExt::DeleteFrozenPolygon(YsShellPolygonHandle plHd)
{
	PolygonAttrib *plAttrib=GetPolygonAttribMaybeFrozen(plHd);
	if(NULL!=plAttrib && YSTRUE==IsFrozen(plHd) && 0==plAttrib->refCount && 0==plAttrib->refCountFromFrozenEntity)
	{
		int nVt;
		const YsShellVertexHandle *plVtHd;
		GetPolygonMaybeFrozen(nVt,plVtHd,plHd);

		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttribMaybeFrozen(plVtHd[idx]);
			--vtAttrib->refCountFromFrozenEntity;
		}

		return YsShell::DeleteFrozenPolygon(plHd);
	}
	return YSERR;
}

const YsShellExt::VertexAttrib *YsShellExt::GetVertexAttrib(YsShellVertexHandle vtHd) const
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKey(vtHd)))
	{
		return vtAttribArray[vtAttribHd];
	}
	return NULL;
}

YsShellExt::VertexAttrib *YsShellExt::GetVertexAttrib(YsShellVertexHandle vtHd)
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKey(vtHd)))
	{
		return vtAttribArray[vtAttribHd];
	}
	return NULL;
}

YsShellExt::VertexAttrib *YsShellExt::GetVertexAttribMaybeFrozen(YsShellVertexHandle vtHd)
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKeyMaybeFrozen(vtHd)))
	{
		return vtAttribArray.GetObjectMaybeFrozen(vtAttribHd);
	}
	return NULL;
}

const YsShellExt::VertexAttrib *YsShellExt::GetVertexAttribMaybeFrozen(YsShellVertexHandle vtHd) const
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKeyMaybeFrozen(vtHd)))
	{
		return vtAttribArray.GetObjectMaybeFrozen(vtAttribHd);
	}
	return NULL;
}

YSRESULT YsShellExt::SetVertexAttrib(YsShellVertexHandle vtHd,const VertexAttrib &incoming)
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKey(vtHd)))
	{
		vtAttribArray[vtAttribHd]->CopyFrom(incoming);
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellExt::IsVertexUsed(YsShellVertexHandle vtHd) const
{
	YsEditArrayObjectHandle <VertexAttrib> vtAttribHd;
	if(YSOK==vtKeyToVtAttribArrayHandle.FindElement(vtAttribHd,GetSearchKey(vtHd)) &&
	   0<vtAttribArray[vtAttribHd]->refCount)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsShellExt::IsVertexUsedByConstEdge(YsShellVertexHandle vtHd,ConstEdgeHandle ceHd) const
{
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	YSBOOL isLoop;
	GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);
	return YsIsIncluded(nCeVt,ceVtHd,vtHd);
}

YSBOOL YsShellExt::IsVertexUsedByNonManifoldEdge(YsShellVertexHandle vtHd) const
{
	for(auto connVtHd : GetConnectedVertex(vtHd))
	{
		int nUsingPl=GetNumPolygonUsingEdge(connVtHd,vtHd);
		if(0!=nUsingPl && 2!=nUsingPl)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsShellExt::IsEdgePieceUsedByFaceGroup(YsShell::Edge edge,FaceGroupHandle fgHd) const
{
	for(auto plHd : FindPolygonFromEdgePiece(edge))
	{
		if(fgHd==FindFaceGroupFromPolygon(plHd))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

const YsShellExt::PolygonAttrib *YsShellExt::GetPolygonAttrib(YsShellPolygonHandle plHd) const
{
	YsEditArrayObjectHandle <PolygonAttrib> plAttribHd;
	if(YSOK==plKeyToPlAttribArrayHandle.FindElement(plAttribHd,GetSearchKey(plHd)))
	{
		return plAttribArray[plAttribHd];
	}
	return NULL;
}

YsShellExt::PolygonAttrib *YsShellExt::GetEditablePolygonAttrib(YsShellPolygonHandle plHd)
{
	YsEditArrayObjectHandle <PolygonAttrib> plAttribHd;
	if(YSOK==plKeyToPlAttribArrayHandle.FindElement(plAttribHd,GetSearchKey(plHd)))
	{
		return plAttribArray[plAttribHd];
	}
	return NULL;
}

YsShellExt::PolygonAttrib *YsShellExt::GetPolygonAttribMaybeFrozen(YsShellPolygonHandle plHd)
{
	YsEditArrayObjectHandle <PolygonAttrib> plAttribHd;
	if(YSOK==plKeyToPlAttribArrayHandle.FindElement(plAttribHd,GetSearchKeyMaybeFrozen(plHd)))
	{
		return plAttribArray.GetObjectMaybeFrozen(plAttribHd);
	}
	return NULL;
}

const YsShellExt::PolygonAttrib *YsShellExt::GetPolygonAttribMaybeFrozen(YsShellPolygonHandle plHd) const
{
	YsEditArrayObjectHandle <PolygonAttrib> plAttribHd;
	if(YSOK==plKeyToPlAttribArrayHandle.FindElement(plAttribHd,GetSearchKeyMaybeFrozen(plHd)))
	{
		return plAttribArray.GetObjectMaybeFrozen(plAttribHd);
	}
	return NULL;
}

YSRESULT YsShellExt::SetPolygonAttrib(YsShellPolygonHandle plHd,const PolygonAttrib &inAttrib)
{
	PolygonAttrib *outAttrib=GetEditablePolygonAttrib(plHd);
	outAttrib->CopyFrom(inAttrib);

	return YSOK;
}

YSSIZE_T YsShellExt::GetNumConstEdge(void) const
{
	return constEdgeArray.GetN();
}

YSHASHKEY YsShellExt::GetSearchKey(ConstEdgeHandle ceHd) const
{
	return constEdgeArray.GetSearchKey(ceHd);
}

YSRESULT YsShellExt::MoveToNextConstEdge(ConstEdgeHandle &ceHd) const
{
	return constEdgeArray.MoveToNext(ceHd);
}

YSRESULT YsShellExt::MoveToPrevConstEdge(ConstEdgeHandle &ceHd) const
{
	return constEdgeArray.MoveToPrev(ceHd);
}

YSRESULT YsShellExt::GetConstEdge(YSSIZE_T &nCeVt,const YsShellVertexHandle *&ceVtHd,YSBOOL &isLoop,ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp)
	{
		isLoop=ceProp->IsLoop();
		nCeVt=ceProp->vtHdArray.GetN();
		ceVtHd=ceProp->vtHdArray;
		return YSOK;
	}
	nCeVt=0;
	ceVtHd=NULL;
	isLoop=YSFALSE;
	return YSERR;
}

YsConstArrayMask <YsShell::VertexHandle> YsShellExt::GetConstEdgeVertex(ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp)
	{
		YsConstArrayMask <YsShell::VertexHandle> ceVtHd(ceProp->vtHdArray.GetN(),ceProp->vtHdArray);
		return ceVtHd;
	}
	YsConstArrayMask <YsShell::VertexHandle> ceVtHd(0,nullptr);
	return ceVtHd;
}

YSBOOL YsShellExt::IsConstEdgeLoop(ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp)
	{
		return ceProp->IsLoop();
	}
	return YSTFUNKNOWN;
}

YsShellExt::ConstEdgeHandle YsShellExt::AddConstEdge(YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop)
{
	ConstEdgeHandle newCeHd=constEdgeArray.Create();
	if(NULL!=newCeHd)
	{
		ConstEdge *ceProp=constEdgeArray[newCeHd];
		ceProp->Initialize();
		ceProp->SetIsLoop(isLoop);
		ceProp->vtHdArray.Set(nVt,ceVtHd);

		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceVtHd[idx]);
			++vtAttrib->refCount;
		}

		extSearch->ConstEdgeAdded(newCeHd);
	}
	return newCeHd;
}

YsShellExt::ConstEdgeHandle YsShellExt::AddConstEdge(const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop)
{
	return AddConstEdge(ceVtHd.GetN(),ceVtHd,isLoop);
}

YsShellExt::ConstEdgeAttrib YsShellExt::GetConstEdgeAttrib(YsShellExt::ConstEdgeHandle ceHd) const
{
	ConstEdgeAttrib attrib;
	auto *ceProp=dynamic_cast <const ConstEdgeAttrib *>(constEdgeArray[ceHd]);
	if(NULL!=ceProp)
	{
		attrib=(*ceProp);
	}
	return attrib;
}

YSRESULT YsShellExt::GetConstEdgeEndPoint(YsVec3 p[2],ConstEdgeHandle ceHd) const
{
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	YSBOOL isLoop;
	GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);
	if(YSTRUE!=isLoop)
	{
		p[0]=GetVertexPosition(ceVtHd[0]);
		p[1]=GetVertexPosition(ceVtHd[nCeVt-1]);
		return YSOK;
	}
	else
	{
		p[0]=GetVertexPosition(ceVtHd[0]);
		p[1]=p[0];
		return YSERR;
	}
}

YSRESULT YsShellExt::SetConstEdgeAttrib(YsShellExt::ConstEdgeHandle ceHd,const YsShellExt::ConstEdgeAttrib &attrib)
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp)
	{
		const int ident=ceProp->ident;
		const int refCount=ceProp->refCount;
		const int refCountFromFrozenEntity=ceProp->refCountFromFrozenEntity;

		ceProp->CopyAttribFrom(attrib);

		ceProp->ident=ident;
		ceProp->refCount=refCount;
		ceProp->refCountFromFrozenEntity=refCountFromFrozenEntity;

		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellExt::GetConstEdgeIsLoop(ConstEdgeHandle ceHd) const
{
	auto ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		return ceProp->IsLoop();
	}
	return YSFALSE;
}

YSRESULT YsShellExt::SetConstEdgeIsLoop(ConstEdgeHandle ceHd,YSBOOL isLoop)
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		ceProp->SetIsLoop(isLoop);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::ModifyConstEdge(ConstEdgeHandle ceHd,YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[],YSBOOL isLoop)
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		extSearch->ConstEdgeAboutToBeDeleted(ceHd);

		for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			--vtAttrib->refCount;
		}

		ceProp->vtHdArray.Set(nVt,ceVtHd);
		if(YSTFUNKNOWN!=isLoop)
		{
			ceProp->SetIsLoop(isLoop);
		}

		for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			++vtAttrib->refCount;
		}

		extSearch->ConstEdgeAdded(ceHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::ModifyConstEdge(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,YSBOOL isLoop)
{
	return ModifyConstEdge(ceHd,ceVtHd.GetN(),ceVtHd,isLoop);
}

YSRESULT YsShellExt::AddConstEdgeVertex(ConstEdgeHandle ceHd,YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[])
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		YSSIZE_T idx0=ceProp->vtHdArray.GetN();
		ceProp->vtHdArray.Append(nVt,ceVtHd);
		for(YSSIZE_T idx=idx0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			++vtAttrib->refCount;
		}

		extSearch->ConstEdgeVertexAdded(ceHd,nVt,ceVtHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::AddConstEdgeVertex(ConstEdgeHandle ceHd,const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd)
{
	return AddConstEdgeVertex(ceHd,ceVtHd.GetN(),ceVtHd);
}

YSRESULT YsShellExt::SetConstEdgeLabel(ConstEdgeHandle ceHd,const char label[])
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		ceProp->label=label;
		return YSOK;
	}
	return YSERR;
}

const char *YsShellExt::GetConstEdgeLabel(ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		return ceProp->label;
	}
	return "";
}

YSRESULT YsShellExt::SetConstEdgeIdent(ConstEdgeHandle ceHd,YSSIZE_T ident)
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		ceProp->ident=(int)ident;
		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsShellExt::GetConstEdgeIdent(ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		return ceProp->ident;
	}
	return -1;
}

YSRESULT YsShellExt::FreezeConstEdge(YsShellExt::ConstEdgeHandle ceHd)
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && 0==ceProp->refCount)
	{
		extSearch->ConstEdgeAboutToBeDeleted(ceHd);

		for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			--vtAttrib->refCount;
			++vtAttrib->refCountFromFrozenEntity;
		}

		constEdgeArray.Freeze(ceHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::MeltConstEdge(ConstEdgeHandle ceHd)
{
	if(YSTRUE==constEdgeArray.IsFrozen(ceHd))
	{
		constEdgeArray.Melt(ceHd);

		const ConstEdge *ceProp=constEdgeArray[ceHd];
		for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			++vtAttrib->refCount;
			--vtAttrib->refCountFromFrozenEntity;
		}

		extSearch->ConstEdgeAdded(ceHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteConstEdge(YsShellExt::ConstEdgeHandle ceHd)
{
	ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd) && 0==ceProp->refCount && 0==ceProp->refCountFromFrozenEntity)
	{
		extSearch->ConstEdgeAboutToBeDeleted(ceHd);

		for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
		{
			VertexAttrib *vtAttrib=GetVertexAttrib(ceProp->vtHdArray[idx]);
			--vtAttrib->refCount;
		}

		constEdgeArray.Delete(ceHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteMultiConstEdge(YSSIZE_T nCe,const ConstEdgeHandle ceHdArray[])
{
	for(YSSIZE_T idx=0; idx<nCe; ++idx)
	{
		DeleteConstEdge(ceHdArray[idx]);
	}
	return YSOK;
}

YSRESULT YsShellExt::DeleteMultiConstEdge(const YsConstArrayMask <ConstEdgeHandle> &ceHdArray)
{
	return DeleteMultiConstEdge(ceHdArray.GetN(),ceHdArray);
}

YSRESULT YsShellExt::DeleteFrozenConstEdge(YsShellExt::ConstEdgeHandle ceHd)
{
	if(YSTRUE==constEdgeArray.IsFrozen(ceHd))
	{
		auto ceProp=constEdgeArray.GetFrozenObject(ceHd);

		if(NULL!=ceProp && 0==ceProp->refCount && 0==ceProp->refCountFromFrozenEntity)
		{
			for(YSSIZE_T idx=0; idx<ceProp->vtHdArray.GetN(); ++idx)
			{
				VertexAttrib *vtAttrib=GetVertexAttribMaybeFrozen(ceProp->vtHdArray[idx]);
				--vtAttrib->refCountFromFrozenEntity;
			}

			constEdgeArray.DeleteFrozen(ceHd);
			return YSOK;
		}
	}
	return YSERR;
}

YSSIZE_T YsShellExt::GetNumFaceGroup(void) const
{
	return faceGroupArray.GetN();
}

YSHASHKEY YsShellExt::GetSearchKey(FaceGroupHandle fgHd) const
{
	return faceGroupArray.GetSearchKey(fgHd);
}

YSRESULT YsShellExt::MoveToNextFaceGroup(FaceGroupHandle &fgHd) const
{
	return faceGroupArray.MoveToNext(fgHd);
}

YsShellExt::FaceGroupHandle YsShellExt::FindFirstFaceGroup(void) const
{
	FaceGroupHandle fgHd=nullptr;
	MoveToNextFaceGroup(fgHd);
	return fgHd;
}

YSRESULT YsShellExt::MoveToPrevFaceGroup(FaceGroupHandle &fgHd) const
{
	return faceGroupArray.MoveToPrev(fgHd);
}

YsShellVertexStore YsShellExt::GetFaceGroupVertex(FaceGroupHandle fgHd) const
{
	YsShellVertexStore fgVtHd(*this);
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp)
	{
		for(auto plHd : fgProp->fgPlHd)
		{
			int nPlVt;
			const VertexHandle *plVtHd;
			GetPolygon(nPlVt,plVtHd,plHd);
			fgVtHd.Add(nPlVt,plVtHd);
		}
	}
	return fgVtHd;
}

YsArray <YsShellPolygonHandle> YsShellExt::GetFaceGroup(FaceGroupHandle fgHd) const
{
	YsArray <YsShellPolygonHandle> fgPlHd;
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp)
	{
		for(auto plHd : fgProp->fgPlHd)
		{
			fgPlHd.Add(plHd);
		}
	}
	return fgPlHd;
}

const YsShell::PolygonStore2 &YsShellExt::GetFaceGroupRaw(FaceGroupHandle fgHd) const
{
	return faceGroupArray[fgHd]->fgPlHd;
}

YsShellExt::FaceGroupAttrib YsShellExt::GetFaceGroupAttrib(FaceGroupHandle fgHd) const
{
	FaceGroupAttrib attrib;
	auto *fgProp=(dynamic_cast <const FaceGroupAttrib *>(faceGroupArray[fgHd]));

	if(NULL!=fgProp)
	{
		attrib=(*fgProp);
	}

	// Thought the following (move and move back) was a good idea, but turned out it makes thread unsafe.
	// if(NULL!=fgProp)
	// {
	// 	FaceGroup *nonConstFgProp=(FaceGroup *)fgProp;
	// 	YsArray <YsShellPolygonHandle> plHdArrayEscape;
	// 	plHdArrayEscape.MoveFrom(nonConstFgProp->plHdArray);
	// 	attrib=(*fgProp);
	// 	nonConstFgProp->plHdArray.MoveFrom(plHdArrayEscape);
	// }
	return attrib;
}

YSRESULT YsShellExt::SetFaceGroupAttrib(YsShellExt::FaceGroupHandle fgHd,const FaceGroupAttrib &attrib)
{
	auto fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp)
	{
		const int ident=fgProp->ident;
		const int refCount=fgProp->refCount;
		const int refCountFromFrozenEntity=fgProp->refCountFromFrozenEntity;

		fgProp->CopyAttribFrom(attrib);

		fgProp->ident=ident;
		fgProp->refCount=refCount;
		fgProp->refCountFromFrozenEntity=refCountFromFrozenEntity;

		return YSOK;
	}
	return YSERR;
}

YsShellExt::FaceGroupHandle YsShellExt::AddFaceGroup(YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	FaceGroupHandle newFgHd=faceGroupArray.Create();
	if(NULL!=newFgHd)
	{
		// Should check if fgPlHd[idx] is already used by a face group.
		if(NULL!=extSearch)
		{
			for(YSSIZE_T idx=0; idx<nPl; ++idx)
			{
				if(NULL!=extSearch->FindFaceGroupFromPolygon(fgPlHd[idx]))
				{
					return NULL;
				}
			}
		}

		FaceGroup *fgProp=faceGroupArray[newFgHd];
		fgProp->Initialize();
		fgProp->fgPlHd.Add(nPl,fgPlHd);

		for(YSSIZE_T idx=0; idx<nPl; ++idx)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(fgPlHd[idx]);
			++plAttrib->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->FaceGroupAdded(newFgHd);
		}

		return newFgHd;
	}
	return NULL;
}

YSRESULT YsShellExt::ModifyFaceGroup(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		if(NULL!=extSearch)
		{
			for(YSSIZE_T idx=0; idx<nPl; ++idx)
			{
				YsShellExt::FaceGroupHandle usingFgHd=extSearch->FindFaceGroupFromPolygon(fgPlHd[idx]);
				if(NULL!=usingFgHd && usingFgHd!=fgHd)
				{
					return YSERR;
				}
			}

			extSearch->FaceGroupAboutToBeDeleted(fgHd);
		}

		for(auto plHd : fgProp->fgPlHd)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
			--plAttrib->refCount;
		}
		fgProp->fgPlHd.CleanUp();
		fgProp->fgPlHd.Add(nPl,fgPlHd);

		for(auto plHd : fgProp->fgPlHd)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
			++plAttrib->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->FaceGroupAdded(fgHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::ModifyFaceGroup(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd)
{
	return ModifyFaceGroup(fgHd,fgPlHd.GetN(),fgPlHd);
}

YSRESULT YsShellExt::AddFaceGroupPolygon(FaceGroupHandle fgHd,const YSSIZE_T nPl,const YsShellPolygonHandle fgPlHd[])
{
	if(0>=nPl)
	{
		return YSOK;
	}

	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		if(NULL!=extSearch)
		{
			for(YSSIZE_T idx=0; idx<nPl; ++idx)
			{
				YsShellExt::FaceGroupHandle usingFgHd=extSearch->FindFaceGroupFromPolygon(fgPlHd[idx]);
				if(NULL!=usingFgHd)
				{
					return YSERR;
				}
			}
		}

		fgProp->fgPlHd.Add(nPl,fgPlHd);
		for(YSSIZE_T idx=0; idx<nPl; ++idx)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(fgPlHd[idx]);
			++plAttrib->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->MultiplePolygonAddedToFaceGroup(fgHd,nPl,fgPlHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::AddFaceGroupPolygon(FaceGroupHandle fgHd,const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd)
{
	return AddFaceGroupPolygon(fgHd,fgPlHd.GetN(),fgPlHd);
}

YsShellExt::FaceGroupHandle YsShellExt::AddFaceGroup(const YsConstArrayMask <YsShellPolygonHandle> &fgPlHd)
{
	return AddFaceGroup(fgPlHd.GetN(),fgPlHd);
}

YsShellExt::FaceGroupHandle YsShellExt::DeleteFaceGroupPolygon(YsShellPolygonHandle plHd)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
	if(NULL!=plAttrib && YSTRUE!=IsFrozen(plHd) && 0<plAttrib->refCount)
	{
		YsShellExt::FaceGroupHandle fgHd=FindFaceGroupFromPolygon(plHd);
		if(NULL!=fgHd)
		{
			DeleteFaceGroupPolygon(fgHd,plHd);
			return fgHd;
		}
	}
	return NULL;
}

YSRESULT YsShellExt::DeleteFaceGroupPolygonMulti(YSSIZE_T nPl,const YsShellPolygonHandle plHdPtr[])
{
	YSRESULT res=YSERR;

	FaceGroupStore fgHdToMod(*this);
	YsShellPolygonStore allPlHd(Conv());
	allPlHd.Add(nPl,plHdPtr);
	for(auto plHd : allPlHd)
	{
		auto fgHd=FindFaceGroupFromPolygon(plHd);
		if(nullptr!=fgHd)
		{
			DeleteFaceGroupPolygon(fgHd,plHd);
			res=YSOK;
		}
	}

	return res;
}

YSRESULT YsShellExt::DeleteFaceGroupPolygonMulti(const YsConstArrayMask <YsShellPolygonHandle> &plHdArray)
{
	return DeleteFaceGroupPolygonMulti(plHdArray.GetN(),plHdArray);
}

void YsShellExt::DeleteFaceGroupPolygon(FaceGroupHandle fgHd,YsShell::PolygonHandle plHd)
{
	PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);

	faceGroupArray[fgHd]->fgPlHd.Delete(plHd);
	--(plAttrib->refCount);
	if(NULL!=extSearch)
	{
		extSearch->PolygonRemovedFromFaceGroup(fgHd,plHd);
	}
}

YSRESULT YsShellExt::SetFaceGroupInteriorConstSurfaceFlag(FaceGroupHandle fgHd,YSBOOL isInteriorConstSurface)
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		unsigned int flags=(unsigned int)fgProp->flags;
		if(YSTRUE==isInteriorConstSurface)
		{
			flags|=FaceGroup::INTERIOR_CONST_SURFACE;
		}
		else
		{
			flags&=~FaceGroup::INTERIOR_CONST_SURFACE;
		}
		fgProp->flags=(FaceGroup::FGFLAG)flags;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::SetFaceGroupLabel(FaceGroupHandle fgHd,const char label[])
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		fgProp->label=label;
		return YSOK;
	}
	return YSERR;
}

const char *YsShellExt::GetFaceGroupLabel(FaceGroupHandle fgHd) const
{
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		return fgProp->label;
	}
	return "";
}

YSRESULT YsShellExt::SetFaceGroupIdent(FaceGroupHandle fgHd,YSSIZE_T ident)
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		fgProp->ident=(int)ident;
		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsShellExt::GetFaceGroupIdent(FaceGroupHandle fgHd) const
{
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		return fgProp->ident;
	}
	return -1;
}

YSRESULT YsShellExt::SetFaceGroupTemporaryFlag(YSSIZE_T nFg,const FaceGroupHandle fgHd[],YSBOOL isTemporary)
{
	for(YSSIZE_T idx=0; idx<nFg; ++idx)
	{
		FaceGroup *fgProp=faceGroupArray[fgHd[idx]];
		if(NULL!=fgProp)
		{
			unsigned int flags=(unsigned int)fgProp->flags;
			if(YSTRUE==isTemporary)
			{
				flags|=((unsigned int)FaceGroup::ISTEMPORARY);
			}
			else
			{
				flags&=(~(unsigned int)FaceGroup::ISTEMPORARY);
			}
			fgProp->flags=(FaceGroup::FGFLAG)flags;
		}
	}
	return YSOK;
}

YSRESULT YsShellExt::SetFaceGroupTemporaryFlag(const YsConstArrayMask <FaceGroupHandle> &fgHdArray,YSBOOL isTemporary)
{
	return SetFaceGroupTemporaryFlag(fgHdArray.GetN(),fgHdArray,isTemporary);
}

YSBOOL YsShellExt::IsFaceGroupTemporary(FaceGroupHandle fgHd) const
{
	auto fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp)
	{
		unsigned int flags=(unsigned int)fgProp->flags;
		if(0!=(flags&((unsigned int)FaceGroup::ISTEMPORARY)))
		{
			return YSTRUE;
		}
	}
	return YSFALSE; 
}

YSBOOL YsShellExt::IsInteriorConstSurface(FaceGroupHandle fgHd) const
{
	const FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp &&
	   YSTRUE!=faceGroupArray.IsFrozen(fgHd) &&
	   0!=(((unsigned int)fgProp->flags)&FaceGroup::INTERIOR_CONST_SURFACE))
	{
		return YSFALSE;
	}
	return YSFALSE;
}

YSRESULT YsShellExt::FreezeFaceGroup(YsShellExt::FaceGroupHandle fgHd)
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		if(NULL!=extSearch)
		{
			extSearch->FaceGroupAboutToBeDeleted(fgHd);
		}

		for(auto plHd : fgProp->fgPlHd)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
			--plAttrib->refCount;
			++plAttrib->refCountFromFrozenEntity;
		}

		faceGroupArray.Freeze(fgHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::MeltFaceGroup(YsShellExt::FaceGroupHandle fgHd)
{
	if(YSTRUE==faceGroupArray.IsFrozen(fgHd))
	{
		faceGroupArray.Melt(fgHd);

		FaceGroup *fgProp=faceGroupArray[fgHd];
		for(auto plHd : fgProp->fgPlHd)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
			++plAttrib->refCount;
			--plAttrib->refCountFromFrozenEntity;
		}

		if(NULL!=extSearch)
		{
			extSearch->FaceGroupAdded(fgHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteFaceGroup(YsShellExt::FaceGroupHandle fgHd)
{
	FaceGroup *fgProp=faceGroupArray[fgHd];
	if(NULL!=fgProp && YSTRUE!=faceGroupArray.IsFrozen(fgHd))
	{
		if(NULL!=extSearch)
		{
			extSearch->FaceGroupAboutToBeDeleted(fgHd);
		}

		for(auto plHd : fgProp->fgPlHd)
		{
			PolygonAttrib *plAttrib=GetEditablePolygonAttrib(plHd);
			--plAttrib->refCount;
		}

		faceGroupArray.Delete(fgHd);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteFrozenFaceGroup(FaceGroupHandle fgHd)
{
	if(YSTRUE==faceGroupArray.IsFrozen(fgHd))
	{
		const FaceGroup *fgProp=faceGroupArray.GetFrozenObject(fgHd);
		if(NULL!=fgProp)
		{
			for(auto plHd : fgProp->fgPlHd)
			{
				PolygonAttrib *plAttrib=GetPolygonAttribMaybeFrozen(plHd);
				--plAttrib->refCountFromFrozenEntity;
			}
			faceGroupArray.DeleteFrozen(fgHd);
			return YSOK;
		}
	}
	return YSERR;

}

YSBOOL YsShellExt::IsEdgeOnFaceGroupBoundary(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	auto edPlHd=FindPolygonFromEdgePiece(edVtHd0,edVtHd1);
	if(0<edPlHd.GetN())
	{
		FaceGroupHandle fgHd0=NULL;
		for(auto plHd : edPlHd)
		{
			FaceGroupHandle fgHd1=FindFaceGroupFromPolygon(plHd);
			if(NULL==fgHd0)
			{
				fgHd0=fgHd1;
			}
			else
			{
				
				if(NULL!=fgHd1 && fgHd0!=fgHd1)
				{
					return YSTRUE;
				}
			}
		}

		if(1==edPlHd.GetN() && NULL!=fgHd0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsShellExt::IsEdgeOnFaceGroupBoundary(const YsShellVertexHandle edVtHd[2]) const
{
	return IsEdgeOnFaceGroupBoundary(edVtHd[0],edVtHd[1]);
}

YSRESULT YsShellExt::MoveToNextVolume(VolumeHandle &vlHd) const
{
	return volumeArray.MoveToNext(vlHd);
}

YSRESULT YsShellExt::MoveToPrevVolume(VolumeHandle &vlHd) const
{
	return volumeArray.MoveToPrev(vlHd);
}

YSSIZE_T YsShellExt::GetNumVolume(void) const
{
	return volumeArray.GetN();
}

YSHASHKEY YsShellExt::GetSearchKey(VolumeHandle vlHd) const
{
	return volumeArray.GetSearchKey(vlHd);
}

YSRESULT YsShellExt::GetVolume(YSSIZE_T &nVlFg,const YsShellExt::FaceGroupHandle *&vlFgHd,YsShellExt::VolumeHandle vlHd) const
{
	const Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp)
	{
		nVlFg=vlProp->fgHdArray.GetN();
		vlFgHd=vlProp->fgHdArray;
		return YSOK;
	}
	nVlFg=0;
	vlFgHd=NULL;
	return YSERR;
}

YsShellExt::Volume YsShellExt::GetVolumeAttrib(YsShellExt::VolumeHandle vlHd) const
{
	Volume attrib;
	const Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp)
	{
		attrib=(*vlProp);
		attrib.fgHdArray.CleanUp();
	}
	return attrib;
}

YSRESULT YsShellExt::SetVolumeAttrib(YsShellExt::VolumeHandle vlHd,const YsShellExt::Volume &attrib)
{
	auto vlProp=volumeArray[vlHd];
	if(NULL!=vlProp)
	{
		const int ident=vlProp->ident;
		const int refCount=vlProp->refCount;
		const int refCountFromFrozenEntity=vlProp->refCountFromFrozenEntity;
		YsArray <FaceGroupHandle> fgHdArray;
		fgHdArray.MoveFrom(vlProp->fgHdArray);

		*vlProp=attrib;

		vlProp->ident=ident;
		vlProp->refCount=refCount;
		vlProp->refCountFromFrozenEntity=refCountFromFrozenEntity;
		vlProp->fgHdArray.MoveFrom(fgHdArray);

		return YSOK;
	}
	return YSERR;
}

YsShellExt::VolumeHandle YsShellExt::AddVolume(YSSIZE_T nVlFg,const YsShellExt::FaceGroupHandle vlFgHd[])
{
	VolumeHandle newVlHd=volumeArray.Create();
	if(NULL!=newVlHd)
	{
		Volume *vlProp=volumeArray[newVlHd];
		vlProp->Initialize();
		vlProp->fgHdArray.Set(nVlFg,vlFgHd);

		for(YSSIZE_T idx=0; idx<nVlFg; ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlFgHd[idx]];
			++fgProp->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->VolumeAdded(newVlHd);
		}

		return newVlHd;
	}
	return NULL;
}

YSRESULT YsShellExt::ModifyVolume(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[])
{
	Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp && YSTRUE!=volumeArray.IsFrozen(vlHd))
	{
		if(NULL!=extSearch)
		{
			extSearch->VolumeAdded(vlHd);
		}

		for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			--fgProp->refCount;
		}

		vlProp->fgHdArray.Set(nVlFg,vlFgHd);

		for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			++fgProp->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->VolumeAdded(vlHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::AddVolumeFaceGroup(VolumeHandle vlHd,YSSIZE_T nVlFg,const FaceGroupHandle vlFgHd[])
{
	Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp && YSTRUE!=volumeArray.IsFrozen(vlHd))
	{
		YSSIZE_T idx0=vlProp->fgHdArray.GetN();
		vlProp->fgHdArray.Append(nVlFg,vlFgHd);
		for(YSSIZE_T idx=idx0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			++fgProp->refCount;
		}

		if(NULL!=extSearch)
		{
			extSearch->MultipleFaceGroupAddedToVolume(vlHd,nVlFg,vlFgHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::FreezeVolume(YsShellExt::VolumeHandle vlHd)
{
	if(NULL!=vlHd && YSTRUE!=volumeArray.IsFrozen(vlHd))
	{
		if(NULL!=extSearch)
		{
			extSearch->VolumeAboutToBeDeleted(vlHd);
		}

		Volume *vlProp=volumeArray[vlHd];

		for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			--fgProp->refCount;
			++fgProp->refCountFromFrozenEntity;
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::MeltVolume(YsShellExt::VolumeHandle vlHd)
{
	if(NULL!=vlHd && YSTRUE==volumeArray.IsFrozen(vlHd))
	{
		volumeArray.Melt(vlHd);

		Volume *vlProp=volumeArray[vlHd];
		for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			++fgProp->refCount;
			--fgProp->refCountFromFrozenEntity;
		}

		if(NULL!=extSearch)
		{
			extSearch->VolumeAdded(vlHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteVolume(YsShellExt::VolumeHandle vlHd)
{
	Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp && YSTRUE!=volumeArray.IsFrozen(vlHd))
	{
		if(NULL!=extSearch)
		{
			extSearch->VolumeAboutToBeDeleted(vlHd);
		}

		for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
		{
			FaceGroup *fgProp=faceGroupArray[vlProp->fgHdArray[idx]];
			--fgProp->refCount;
		}
		volumeArray.Delete(vlHd);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt::DeleteFrozenVolume(YsShellExt::VolumeHandle vlHd)
{
	if(YSTRUE==volumeArray.IsFrozen(vlHd))
	{
		Volume *vlProp=volumeArray.GetObjectMaybeFrozen(vlHd);
		if(NULL!=vlProp)
		{
			for(YSSIZE_T idx=0; idx<vlProp->fgHdArray.GetN(); ++idx)
			{
				FaceGroup *fgProp=faceGroupArray.GetObjectMaybeFrozen(vlProp->fgHdArray[idx]);
				--fgProp->refCountFromFrozenEntity;
			}
			volumeArray.DeleteFrozen(vlHd);
			return YSOK;
		}
	}
	return YSERR;
}

#include "ysshellextsearch.h"

YSRESULT YsShellExt::SetVolumeLabel(VolumeHandle vlHd,const char label[])
{
	Volume *vlProp=volumeArray[vlHd];
	if(NULL!=vlProp && YSTRUE!=volumeArray.IsFrozen(vlHd))
	{
		vlProp->label=label;
		return YSOK;
	}
	return YSERR;
}

YSSIZE_T YsShellExt::GetNumEdge(void) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNumEdge(*this);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return 0;
}

YSRESULT YsShellExt::MoveToNextEdge(YsShellEdgeEnumHandle &handle) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->FindNextEdge(*this,handle);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return YSERR;
}

YSRESULT YsShellExt::GetEdge(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle handle) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetEdge(*this,edVtHd1,edVtHd2,handle);
	}
	edVtHd1=NULL;
	edVtHd2=NULL;
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return YSERR;
}

YSRESULT YsShellExt::GetEdge(YsShellVertexHandle edVtHd[2],YsShellEdgeEnumHandle edHd) const
{
	return GetEdge(edVtHd[0],edVtHd[1],edHd);
}

YsShell::Edge YsShellExt::GetEdge(YsShellEdgeEnumHandle handle) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetEdge(*this,handle);
	}
	Edge edge;
	edge.edVtHd[0]=NULL;
	edge.edVtHd[1]=NULL;
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return edge;
}

YSRESULT YsShellExt::EnableSearch(void)
{
	if(NULL==shlSearch && NULL==extSearch)
	{
		shlSearch=new YsShellSearchTable;
		AttachSearchTable(shlSearch);
		extSearch=new SearchTable;
		extSearch->Attach(this);
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellExt::IsSearchEnabled(void) const
{
	if(NULL!=shlSearch && NULL!=extSearch)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT YsShellExt::DisableSearch(void)
{
	if(NULL!=shlSearch)
	{
		YsShell::DetachSearchTable();
		delete shlSearch;
		shlSearch=NULL;
	}
	if(NULL!=extSearch)
	{
		extSearch->Detach();
		delete extSearch;
		extSearch=NULL;
	}
	return YSOK;
}

YSRESULT YsShellExt::TestSearchTable(void) const
{
	YSRESULT res=YSOK;
	if(NULL!=extSearch)
	{
		// Const-edge to vertex
		ConstEdgeHandle ceHd=NULL;
		while(YSOK==MoveToNextConstEdge(ceHd))
		{
			YSSIZE_T nVt;
			const YsShellVertexHandle *ceVtHd;
			YSBOOL isLoop;

			if(YSOK==GetConstEdge(nVt,ceVtHd,isLoop,ceHd))
			{
				for(YSSIZE_T idx=0; idx<nVt; ++idx)
				{
					YsArray <ConstEdgeHandle,16> vtCeHd;
					if(YSOK==extSearch->FindConstEdgeFromVertex <16> (vtCeHd,ceVtHd[idx]))
					{
						if(YSTRUE!=vtCeHd.IsIncluded(ceHd))
						{
							printf("%s %d\n",__FUNCTION__,__LINE__);
							printf("Error!  Const-edge to vertex table does not include what's supposed to be included.\n");
							res=YSERR;
						}
					}
					else
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  Const-edge cannot be found from a vertex.\n");
						res=YSERR;
					}
				}
			}
		}

		// Vertex to const edge
		YsShellVertexHandle vtHd=NULL;
		while(YSOK==MoveToNextVertex(vtHd))
		{
			YsArray <ConstEdgeHandle,16> vtCeHd;
			if(YSOK==extSearch->FindConstEdgeFromVertex(vtCeHd,vtHd))
			{
				for(YSSIZE_T idx=0; idx<vtCeHd.GetN(); ++idx)
				{
					YSSIZE_T nVt;
					const YsShellVertexHandle *ceVtHd;
					YSBOOL isLoop;

					if(YSOK==GetConstEdge(nVt,ceVtHd,isLoop,vtCeHd[idx]))
					{
						if(YSTRUE!=YsIsIncluded(nVt,ceVtHd,vtHd))
						{
							printf("%s %d\n",__FUNCTION__,__LINE__);
							printf("Error!  Const-edge is found from a vertex, but the const-edge is not using the vertex.\n");
							res=YSERR;
						}
					}
					else
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  Const-edge is found from a vertex, but the const-edge does not exist.\n");
						res=YSERR;
					}
				}
			}
		}

		// FaceGroup to Polygon
		for(FaceGroupHandle fgHd=NULL; YSOK==MoveToNextFaceGroup(fgHd); )
		{
			{
				for(auto plHd : GetFaceGroup(fgHd))
				{
					FaceGroupHandle found=extSearch->FindFaceGroupFromPolygon(plHd);
					if(found!=fgHd)
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  Polygon to face group table is corrupted.\n");
						res=YSERR;
					}
				}
				for(auto plHd : GetFaceGroupRaw(fgHd))
				{
					FaceGroupHandle found=extSearch->FindFaceGroupFromPolygon(plHd);
					if(found!=fgHd)
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  Polygon to face group table is corrupted.\n");
						res=YSERR;
					}
				}
			}
		}

		// Polygon to FaceGroup
		for(YsShellPolygonHandle plHd=NULL; YSOK==MoveToNextPolygon(plHd); )
		{
			FaceGroupHandle fgHd=extSearch->FindFaceGroupFromPolygon(plHd);
			if(NULL!=fgHd)
			{
				auto &fgPlHd=GetFaceGroupRaw(fgHd);
				if(YSTRUE!=fgPlHd.IsIncluded(plHd))
				{
					printf("%s %d\n",__FUNCTION__,__LINE__);
					printf("Error!  Polygon to FaceGroup table is corrupted.\n");
					res=YSERR;
				}
			}
		}

		// Volume to FaceGroup
		for(VolumeHandle vlHd=NULL; YSOK==MoveToNextVolume(vlHd); )
		{
			YSSIZE_T nVlFg;
			const YsShellExt::FaceGroupHandle *vlFgHd;
			if(YSOK==GetVolume(nVlFg,vlFgHd,vlHd))
			{
				for(YSSIZE_T idx=0; idx<nVlFg; ++idx)
				{
					YSSIZE_T nFound=0;
					const VolumeHandle *found=NULL;
					if(YSOK!=extSearch->FindVolumeFromFaceGroup(nFound,found,vlFgHd[idx]) ||
					   YSTRUE!=YsIsIncluded(nFound,found,vlHd))
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  Face group to volume table is corrupted.\n");
						res=YSERR;
					}
				}
			}
			else
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
				printf("Error!  Cannot get volume face groups.\n");
				res=YSERR;
			}
		}

		// FaceGroup to Volume
		for(YsShellExt::FaceGroupHandle fgHd=NULL; YSOK==MoveToNextFaceGroup(fgHd); )
		{
			YSSIZE_T nVl;
			const YsShellExt::VolumeHandle *vlHd;

			if(YSOK==extSearch->FindVolumeFromFaceGroup(nVl,vlHd,fgHd))
			{
				for(YSSIZE_T vlIdx=0; vlIdx<nVl; ++vlIdx)
				{
					YSSIZE_T nVlFg;
					const YsShellExt::FaceGroupHandle *vlFgHd;
					GetVolume(nVlFg,vlFgHd,vlHd[vlIdx]);
					if(YSTRUE!=YsIsIncluded(nVlFg,vlFgHd,fgHd))
					{
						printf("%s %d\n",__FUNCTION__,__LINE__);
						printf("Error!  FaceGroup to Volume table is corrupted.\n");
						res=YSERR;
					}
				}
			}
		}
	}

	return res;
}

YsShellVertexHandle YsShellExt::FindVertex(YSHASHKEY vtKey) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->FindVertex(*this,vtKey);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return NULL;
}

YsShellPolygonHandle YsShellExt::FindPolygon(YSHASHKEY plKey) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->FindPolygon(*this,plKey);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return NULL;
}

YsShell::PolygonHandle YsShellExt::FindPolygonFromVertexSequence(YSSIZE_T nVt,const YsShellVertexHandle vtHd[]) const
{
	if(3<=nVt)
	{
		auto edPlHd=FindPolygonFromEdgePiece(vtHd[0],vtHd[1]);
		for(auto plHd : edPlHd)
		{
			int nPlVt;
			const YsShell::VertexHandle *plVtHd;
			GetPolygon(nPlVt,plVtHd,plHd);
			const YSBOOL orientationSensitive=YSFALSE;
			if(YSTRUE==YsSamePolygon(nVt,vtHd,nPlVt,plVtHd,orientationSensitive))
			{
				return plHd;
			}
		}
	}
	return nullptr;
}

YsShell::PolygonHandle YsShellExt::FindPolygonFromVertexSequence(const YsConstArrayMask <YsShell::VertexHandle> &plVtHd) const 
{
	return FindPolygonFromVertexSequence(plVtHd.GetN(),plVtHd);
}

int YsShellExt::GetNumPolygonUsingEdge(YsShellEdgeEnumHandle edHd) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNumPolygonUsingEdge(*this,edHd);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return 0;
}

int YsShellExt::GetNumPolygonUsingEdge(const YsShellVertexHandle edVtHd[2]) const
{
	return GetNumPolygonUsingEdge(edVtHd[0],edVtHd[1]);
}

int YsShellExt::GetNumPolygonUsingEdge(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNumPolygonUsingEdge(*this,edVtHd0,edVtHd1);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return 0;
}

int YsShellExt::GetNumPolygonUsingVertex(YsShellVertexHandle vtHd) const
{
	if(NULL!=shlSearch)
	{
		int nVtPl;
		const YsShellPolygonHandle *plHdArray;
		if(YSOK==shlSearch->FindPolygonListByVertex(nVtPl,plHdArray,*this,vtHd))
		{
			return nVtPl;
		}
		return 0;
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return 0;
}

int YsShellExt::GetNumFaceGroupUsingVertex(YsShellVertexHandle vtHd) const
{
	YSSIZE_T nPl;
	const YsShell::PolygonHandle *plHd;
	if(YSOK==FindPolygonFromVertex(nPl,plHd,vtHd))
	{
		YsVisited <YsShellExt,FaceGroupHandle> visited;
		for(YSSIZE_T i=0; i<nPl; ++i)
		{
			auto fgHd=FindFaceGroupFromPolygon(plHd[i]);
			if(nullptr!=fgHd && YSTRUE!=visited.IsIncluded(*this,fgHd))
			{
				visited.Add(*this,fgHd);
			}
		}
		return (int)visited.GetN();
	}
	return 0;
}

YSRESULT YsShellExt::FindPolygonFromVertex(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,YsShellVertexHandle vtHd) const
{
	if(NULL!=shlSearch)
	{
		int nVtPl;
		if(YSOK==shlSearch->FindPolygonListByVertex(nVtPl,plHdArray,*this,vtHd))
		{
			nPl=nVtPl;
			return YSOK;
		}
		return YSERR;
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return YSERR;
}

YsArray <YsShellPolygonHandle,8> YsShellExt::FindSmoothShadingPolygonGroup(YsShellPolygonHandle plHd,YsShellVertexHandle vtHd) const
{
	YsArray <YsShellPolygonHandle,8> todo;
	todo.Append(plHd);
	for(YSSIZE_T todoIdx=0; todoIdx<todo.GetN(); ++todoIdx)
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHdPtr;
		GetPolygon(nPlVt,plVtHdPtr,todo[todoIdx]);

		YsConstArrayMask <YsShellVertexHandle> plVtHd(nPlVt,plVtHdPtr);
		for(YSSIZE_T edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			const YsShellVertexHandle edVtHd[2]={plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1)};
			if((edVtHd[0]==vtHd || edVtHd[1]==vtHd) && YSTRUE!=IsOnCreaseConstEdge(edVtHd))
			{
				YSSIZE_T nEdPl=0;
				const YsShellPolygonHandle *edPlHdPtr=NULL;
				FindPolygonFromEdgePiece(nEdPl,edPlHdPtr,edVtHd);
				YsConstArrayMask <YsShellPolygonHandle> edPlHd(nEdPl,edPlHdPtr);
				for(auto neiPlHd : edPlHd)
				{
					if(YSTRUE!=todo.IsIncluded(neiPlHd))
					{
						todo.Append(neiPlHd);
					}
				}
			}
		}
	}

	return todo;
}

YsArray <YsShellPolygonHandle> YsShellExt::FindPolygonFromVertex(YsShellVertexHandle vtHd) const
{
	YsArray <YsShellPolygonHandle> vtPlHd;
	FindPolygonFromVertex(vtPlHd,vtHd);
	return vtPlHd;
}

YSRESULT YsShellExt::FindPolygonFromEdgePiece(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(NULL!=shlSearch)
	{
		int nEdPl;
		if(YSOK==shlSearch->FindPolygonListByEdge(nEdPl,plHdArray,*this,edVtHd0,edVtHd1))
		{
			nPl=nEdPl;
			return YSOK;
		}
		return YSERR;
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return YSERR;
}

YSRESULT YsShellExt::FindPolygonFromEdgePiece(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdArray,const YsShellVertexHandle edVtHd[2]) const
{
	return FindPolygonFromEdgePiece(nPl,plHdArray,edVtHd[0],edVtHd[1]);
}

YsArray <YsShellPolygonHandle,2> YsShellExt::FindPolygonFromEdgePiece(YsShellEdgeEnumHandle edHd) const
{
	auto edPlHd=shlSearch->FindPolygonListByEdge(Conv(),edHd);
	if(NULL!=edPlHd)
	{
		return *edPlHd;
	}
	YsArray <YsShellPolygonHandle,2> empty;
	return empty;
}

YsArray <YsShellPolygonHandle,2> YsShellExt::FindPolygonFromEdgePiece(YsShell::Edge edge) const
{
	return FindPolygonFromEdgePiece(edge[0],edge[1]);
}

YsArray <YsShellPolygonHandle,2> YsShellExt::FindPolygonFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	YsArray <YsShellPolygonHandle,2> edPlHd;
	FindPolygonFromEdgePiece(edPlHd,edVtHd);
	return edPlHd;
}

YsArray <YsShellPolygonHandle,2> YsShellExt::FindPolygonFromEdgePiece(const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1) const
{
	YsArray <YsShellPolygonHandle,2> edPlHd;
	FindPolygonFromEdgePiece(edPlHd,edVtHd0,edVtHd1);
	return edPlHd;
}

YsShellPolygonHandle YsShellExt::GetNeighborPolygon(YsShellPolygonHandle plHd,YSSIZE_T n) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNeighborPolygon(*this,plHd,n);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return NULL;
}

YsShellPolygonHandle YsShellExt::GetNeighborPolygon(YsShellPolygonHandle plHd,const YsShellVertexHandle edVtHd[2]) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNeighborPolygon(*this,plHd,edVtHd[0],edVtHd[1]);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return NULL;
}

YsShellPolygonHandle YsShellExt::GetNeighborPolygon(YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(NULL!=shlSearch)
	{
		return shlSearch->GetNeighborPolygon(*this,plHd,edVtHd0,edVtHd1);
	}
	fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
	fprintf(stderr,"Called without SearchTable attached.\n");
	return NULL;
}

YsArray <YsShellVertexHandle> YsShellExt::GetConnectedVertex(YsShellVertexHandle fromVtHd) const
{
	YsArray <YsShellVertexHandle> connVtHd;
	GetConnectedVertex(connVtHd,fromVtHd);
	return connVtHd;
}

YsArray <YsShellVertexHandle> YsShellExt::GetConnectedVertex(YsShellVertexHandle fromVtHd,const Condition &cond) const
{
	auto connVtHd=GetConnectedVertex(fromVtHd);
	for(auto idx=connVtHd.GetN()-1; 0<=idx; --idx)
	{
		if(YSOK!=cond.TestEdge(*this,fromVtHd,connVtHd[idx]))
		{
			connVtHd.DeleteBySwapping(idx);
		}
	}
	return connVtHd;
}

int YsShellExt::GetNumConstEdgeConnectedVertex(YsShellVertexHandle fromVtHd) const
{
	auto connVtHd=GetConnectedVertex(fromVtHd);
	int n=0;
	for(auto toVtHd : connVtHd)
	{
		if(0<GetNumConstEdgeUsingEdgePiece(fromVtHd,toVtHd))
		{
			++n;
		}
	}
	return n;
}

YsArray <YsShellVertexHandle> YsShellExt::GetConstEdgeConnectedVertex(YsShellVertexHandle fromVtHd) const
{
	YsArray <YsShellVertexHandle> connVtHd;
	GetConstEdgeConnectedVertex(connVtHd,fromVtHd);
	return connVtHd;
}

YsShellExt::ConstEdgeHandle YsShellExt::FindConstEdge(YSHASHKEY ceKey) const
{
	return constEdgeArray.FindObject(ceKey);
}

YsShellExt::ConstEdgeHandle YsShellExt::GetConstEdgeHandleFromIndex(YSSIZE_T index) const
{
	return constEdgeArray.GetObjectHandleFromIndex(index);
}

YSRESULT YsShellExt::FindConstEdgeFromVertex(YSSIZE_T &nCe,const YsShellExt::ConstEdgeHandle *&ceHdPtr,YsShellVertexHandle vtHd) const
{
	if(NULL!=extSearch)
	{
		return extSearch->FindConstEdgeFromVertex(nCe,ceHdPtr,vtHd);
	}
	return YSERR;
}

YSRESULT YsShellExt::FindConstEdgeFromVertex(YSSIZE_T &nCe,const ConstEdgeHandle *&ceHdPtr,YSHASHKEY vtKey) const
{
	if(NULL!=extSearch)
	{
		return extSearch->FindConstEdgeFromVertex(nCe,ceHdPtr,vtKey);
	}
	return YSERR;
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExt::FindConstEdgeFromVertex(YsShellVertexHandle vtHd) const
{
	YsArray <YsShellExt::ConstEdgeHandle> vtCeHd;
	FindConstEdgeFromVertex(vtCeHd,vtHd);
	return vtCeHd;
}

int YsShellExt::GetNumConstEdgeUsingVertex(YsShellVertexHandle vtHd) const
{
	if(NULL!=extSearch)
	{
		YSSIZE_T nCe;
		const ConstEdgeHandle *ceHdPtr;
		if(YSOK==extSearch->FindConstEdgeFromVertex(nCe,ceHdPtr,vtHd))
		{
			return (int)nCe;
		}
	}
	return 0;
}

YSBOOL YsShellExt::IsEdgePieceConstrained(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(0<GetNumConstEdgeUsingEdgePiece(edVtHd0,edVtHd1))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSBOOL YsShellExt::IsEdgePieceConstrained(const YsShellVertexHandle edVtHd[2]) const
{
	if(0<GetNumConstEdgeUsingEdgePiece(edVtHd))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSBOOL YsShellExt::IsEdgePieceConstrained(YsShellEdgeEnumHandle edHd) const
{
	if(0<GetNumConstEdgeUsingEdgePiece(edHd))
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSBOOL YsShellExt::IsEdgePieceConstrained(YsShell::Edge edge) const
{
	if(0<GetNumConstEdgeUsingEdgePiece(edge))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsShellExt::IsEdgePieceConstrained(YsShell::PolygonHandle plHd,YSSIZE_T edIdx) const
{
	auto edge=GetPolygonEdge(plHd,edIdx);
	if(nullptr!=edge[0])
	{
		return IsEdgePieceConstrained(edge);
	}
	return YSFALSE;
}

YSBOOL YsShellExt::IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	YSBOOL isLoop;

	GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);
	if(2<=nCeVt)
	{
		for(YSSIZE_T edIdx=0; edIdx<nCeVt-1; ++edIdx)
		{
			if(YSTRUE==YsSameEdge(edVtHd0,edVtHd1,ceVtHd[edIdx],ceVtHd[edIdx+1]))
			{
				return YSTRUE;
			}
		}
		if(YSTRUE==isLoop && YSTRUE==YsSameEdge(edVtHd0,edVtHd1,ceVtHd[0],ceVtHd[nCeVt-1]))
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}
YSBOOL YsShellExt::IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,const YsShellVertexHandle edVtHd[2]) const
{
	return IsConstEdgeUsingEdgePiece(ceHd,edVtHd[0],edVtHd[1]);
}
YSBOOL YsShellExt::IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShellEdgeEnumHandle edHd) const
{
	YsShellVertexHandle edVtHd[2];
	GetEdge(edVtHd,edHd);
	return IsConstEdgeUsingEdgePiece(ceHd,edVtHd);
}
YSBOOL YsShellExt::IsConstEdgeUsingEdgePiece(ConstEdgeHandle ceHd,YsShell::Edge edge) const
{
	return IsConstEdgeUsingEdgePiece(ceHd,edge[0],edge[1]);
}

int YsShellExt::GetNumConstEdgeUsingEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YsArray <ConstEdgeHandle,16> ceHdArray;
	if(YSOK==FindConstEdgeFromEdgePiece(ceHdArray,edVtHd0,edVtHd1))
	{
		return (int)ceHdArray.GetN();
	}
	return 0;
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExt::FindConstEdgeFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	YsArray <ConstEdgeHandle> found;
	FindConstEdgeFromEdgePiece(found,edVtHd0,edVtHd1);
	return found;
}
YsArray <YsShellExt::ConstEdgeHandle> YsShellExt::FindConstEdgeFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	YsArray <ConstEdgeHandle> found;
	FindConstEdgeFromEdgePiece(found,edVtHd);
	return found;
}
YsArray <YsShellExt::ConstEdgeHandle>  YsShellExt::FindConstEdgeFromEdgePiece(YSHASHKEY edVtKey0,YSHASHKEY edVtKey1) const
{
	YsArray <ConstEdgeHandle> found;
	FindConstEdgeFromEdgePiece(found,edVtKey0,edVtKey1);
	return found;
}
YsArray <YsShellExt::ConstEdgeHandle>  YsShellExt::FindConstEdgeFromEdgePiece(const YSHASHKEY edVtKey[2]) const
{
	YsArray <ConstEdgeHandle> found;
	FindConstEdgeFromEdgePiece(found,edVtKey);
	return found;
}

YSRESULT YsShellExt::SetConstEdgeIsCrease(ConstEdgeHandle ceHd,YSBOOL isCrease)
{
	auto ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		ceProp->SetIsCrease(isCrease);
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShellExt::IsCreaseConstEdge(ConstEdgeHandle ceHd) const
{
	const ConstEdge *ceProp=constEdgeArray[ceHd];
	if(NULL!=ceProp && YSTRUE!=constEdgeArray.IsFrozen(ceHd))
	{
		return ceProp->IsCrease();
	}
	return YSFALSE;
}
YSBOOL YsShellExt::IsOnCreaseConstEdge(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	auto edCeHd=FindConstEdgeFromEdgePiece(edVtHd0,edVtHd1);
	for(auto ceHd : edCeHd)
	{
		if(YSTRUE==IsCreaseConstEdge(ceHd))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}
YSBOOL YsShellExt::IsOnCreaseConstEdge(const YsShellVertexHandle edVtHd[2]) const
{
	return IsOnCreaseConstEdge(edVtHd[0],edVtHd[1]);
}
YSBOOL YsShellExt::IsOnCreaseConstEdge(const YsShell::Edge &edge) const
{
	return IsOnCreaseConstEdge(edge[0],edge[1]);
}
YSBOOL YsShellExt::IsOnCreaseConstEdge(YsShellVertexHandle vtHd) const
{
	auto vtCeHd=FindConstEdgeFromVertex(vtHd);
	for(auto ceHd : vtCeHd)
	{
		if(YSTRUE==IsCreaseConstEdge(ceHd))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExt::FindConstEdgeFromPolygon(YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	GetPolygon(nPlVt,plVtHd,plHd);

	YsArray <YsShellExt::ConstEdgeHandle> plCeHd;
	for(int edIdx=0; edIdx<nPlVt; ++edIdx)
	{
		const YsShellVertexHandle edVtHd[2]={plVtHd[edIdx],YsGetCyclic(nPlVt,plVtHd,edIdx+1)};
		auto edCeHd=FindConstEdgeFromEdgePiece(edVtHd);
		for(auto ceHd : edCeHd)
		{
			if(YSTRUE!=plCeHd.IsIncluded(ceHd))
			{
				plCeHd.Append(ceHd);
			}
		}
	}
	return plCeHd;
}

YsArray <YsShellExt::ConstEdgeHandle> YsShellExt::FindConstEdgeFromFaceGroup(FaceGroupHandle fgHd) const
{
	auto fgPlHd=GetFaceGroup(fgHd);

	YsArray <ConstEdgeHandle> fgCeHd;
	for(auto plHd : fgPlHd)
	{
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		GetPolygon(nPlVt,plVtHd,plHd);

		for(int edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			const YsShellVertexHandle edVtHd[2]={plVtHd[edIdx],YsGetCyclic(nPlVt,plVtHd,edIdx+1)};
			auto edCeHd=FindConstEdgeFromEdgePiece(edVtHd);
			for(auto ceHd : edCeHd)
			{
				if(8<=fgCeHd.GetN() || YSTRUE!=fgCeHd.IsIncluded(ceHd))
				{
					fgCeHd.Append(ceHd);
				}
			}
		}
	}

	if(8<=fgCeHd.GetN())
	{
		YsRemoveDuplicateInUnorderedArray(fgCeHd);
	}

	return fgCeHd;
}

int YsShellExt::GetNumConstEdgeUsingEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	return GetNumConstEdgeUsingEdgePiece(edVtHd[0],edVtHd[1]);
}

int YsShellExt::GetNumConstEdgeUsingEdgePiece(YsShellEdgeEnumHandle edHd) const
{
	return GetNumConstEdgeUsingEdgePiece(GetEdge(edHd));
}

int YsShellExt::GetNumConstEdgeUsingEdgePiece(YsShell::Edge edge) const
{
	return GetNumConstEdgeUsingEdgePiece(edge.edVtHd[0],edge.edVtHd[1]);
}

YsShellExt::FaceGroupHandle YsShellExt::FindFaceGroup(YSHASHKEY fgKey) const
{
	return faceGroupArray.FindObject(fgKey);
}

YsShellExt::FaceGroupHandle YsShellExt::FindFaceGroupFromPolygon(YsShellPolygonHandle plHd) const
{
	if(NULL!=plHd && NULL!=extSearch)
	{
		return extSearch->FindFaceGroupFromPolygon(plHd);
	}
	return NULL;
}

YsArray <YsShellExt::FaceGroupHandle,4> YsShellExt::FindFaceGroupFromVertex(YsShellVertexHandle vtHd) const
{
	auto vtPlHd=FindPolygonFromVertex(vtHd);
	YsArray <YsShellExt::FaceGroupHandle,4> fgHdArray;
	for(auto plHd : vtPlHd)
	{
		auto fgHd=FindFaceGroupFromPolygon(plHd);
		if(NULL!=fgHd && YSTRUE!=fgHdArray.IsIncluded(fgHd))
		{
			fgHdArray.Append(fgHd);
		}
	}
	return fgHdArray;
}

YsArray <YsShellExt::FaceGroupHandle,2> YsShellExt::FindFaceGroupFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	auto edPlHd=FindPolygonFromEdgePiece(edVtHd0,edVtHd1);
	YsArray <YsShellExt::FaceGroupHandle,2> fgHdArray;
	for(auto plHd : edPlHd)
	{
		auto fgHd=FindFaceGroupFromPolygon(plHd);
		if(NULL!=fgHd && YSTRUE!=fgHdArray.IsIncluded(fgHd))
		{
			fgHdArray.Append(fgHd);
		}
	}
	return fgHdArray;
}

YsArray <YsShellExt::FaceGroupHandle,2> YsShellExt::FindFaceGroupFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	return FindFaceGroupFromEdgePiece(edVtHd[0],edVtHd[1]);
}

YsArray <YsShellExt::FaceGroupHandle,2> YsShellExt::FindFaceGroupFromConstEdge(ConstEdgeHandle ceHd) const
{
	YsArray <FaceGroupHandle,2> ceFgHd;
	YSBOOL needRemoveDuplicate=YSFALSE;

	YSBOOL isLoop;
	YsArray <YsShellVertexHandle,16> ceVtHd;
	GetConstEdge(ceVtHd,isLoop,ceHd);

	if(YSTRUE==isLoop)
	{
		ceVtHd.Append(ceVtHd[0]);
	}

	for(YSSIZE_T idx=0; idx<ceVtHd.GetN()-1; ++idx)
	{
		auto edFgHd=FindFaceGroupFromEdgePiece(ceVtHd[idx],ceVtHd[idx+1]);
		for(auto fgHd : edFgHd)
		{
			if(YSTRUE==needRemoveDuplicate || YSTRUE!=ceFgHd.IsIncluded(fgHd))
			{
				ceFgHd.Append(fgHd);
				if(32<ceFgHd.GetN())
				{
					needRemoveDuplicate=YSTRUE;
				}
			}
		}
	}

	if(YSTRUE==needRemoveDuplicate)
	{
		YsRemoveDuplicateInUnorderedArray(ceFgHd);
	}

	return ceFgHd;
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExt::GetEdgeConnectedNeighborFaceGroup(YsShellExt::FaceGroupHandle fromFgHd) const
{
	auto fgPlHdArray=GetFaceGroup(fromFgHd);

	YsShellPolygonStore seedPlHd(this->Conv());
	seedPlHd.AddPolygon(fgPlHdArray);

	FaceGroupStore neiFgHdStore(*this);

	for(auto fgPlHd : fgPlHdArray)
	{
		auto plVtHd=GetPolygonVertex(fgPlHd);
		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			const YsShellVertexHandle edVtHd[2]=
			{
				plVtHd[edIdx],
				plVtHd.GetCyclic(edIdx+1)
			};

			auto neiPlHdArray=FindPolygonFromEdgePiece(edVtHd);
			for(auto neiPlHd : neiPlHdArray)
			{
				if(YSTRUE!=seedPlHd.IsIncluded(neiPlHd))
				{
					auto neiFgHd=FindFaceGroupFromPolygon(neiPlHd);
					if(NULL!=neiFgHd && neiFgHd!=fromFgHd)
					{
						neiFgHdStore.AddFaceGroup(neiFgHd);
					}
				}
			}
		}
	}

	YsArray <FaceGroupHandle> neiFgHdArray;
	for(auto fgHd : neiFgHdStore)
	{
		neiFgHdArray.Append(fgHd);
	}

	return neiFgHdArray;
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExt::GetVertexConnectedNeighborFaceGroup(FaceGroupHandle fromFgHd) const
{
	auto fgPlHdArray=GetFaceGroup(fromFgHd);

	YsShellVertexStore fgVtHd(this->Conv());
	for(auto plHd : fgPlHdArray)
	{
		auto plVtHd=GetPolygonVertex(plHd);
		fgVtHd.AddVertex(plVtHd);
	}

	FaceGroupStore neiFgHdStore(*this);

	for(auto vtHd : fgVtHd)
	{
		auto vtPlHd=FindPolygonFromVertex(vtHd);
		for(auto plHd : vtPlHd)
		{
			auto fgHd=FindFaceGroupFromPolygon(plHd);
			if(fgHd!=fromFgHd)
			{
				neiFgHdStore.AddFaceGroup(fgHd);
			}
		}
	}

	YsArray <FaceGroupHandle> neiFgHdArray;
	for(auto fgHd : neiFgHdStore)
	{
		neiFgHdArray.Append(fgHd);
	}

	return neiFgHdArray;
}

YsArray <YsShellExt::FaceGroupHandle> YsShellExt::GetConstEdgeConnectedNeighborFaceGroup(FaceGroupHandle fromFgHd,ConstEdgeHandle ceHd) const
{
	YsArray <YsShellExt::FaceGroupHandle> neiFgHd;
	if(NULL==fromFgHd || NULL==ceHd)
	{
		return neiFgHd;
	}


	YsArray <YsShellVertexHandle> ceVtHd;
	YSBOOL isLoop;
	GetConstEdge(ceVtHd,isLoop,ceHd);

	if(YSTRUE==isLoop)
	{
		ceVtHd.Append(ceVtHd[0]);
	}

	for(YSSIZE_T edIdx=0; edIdx<ceVtHd.GetN()-1; ++edIdx)
	{
		auto edPlHd=FindPolygonFromEdgePiece(ceVtHd[edIdx],ceVtHd[edIdx+1]);

		YSBOOL usingFromFgHd=YSFALSE;
		YsArray <FaceGroupHandle,2> edFgHd;
		for(auto plHd : edPlHd)
		{
			auto fgHd=FindFaceGroupFromPolygon(plHd);
			if(fromFgHd==fgHd)
			{
				usingFromFgHd=YSTRUE;
			}
			else if(NULL!=fgHd)
			{
				edFgHd.Append(fgHd);
			}
		}

		if(YSTRUE==usingFromFgHd)
		{
			for(auto fgHd : edFgHd)
			{
				if(8<=neiFgHd.GetN() || YSTRUE!=neiFgHd.IsIncluded(fgHd))
				{
					neiFgHd.Append(fgHd);
				}
			}
		}
	}

	if(8<=neiFgHd.GetN())
	{
		YsRemoveDuplicateInUnorderedArray(neiFgHd);
	}
	return neiFgHd;
}

YsArray <YsShellExt::VertexHandle> YsShellExt::GetFaceGroupInsideVertex(FaceGroupHandle fgHd) const
{
	YsArray <VertexHandle> fgVtHd;
	for(auto plHd : GetFaceGroup(fgHd))
	{
		for(auto vtHd : GetPolygonVertex(plHd))
		{
			if(1==FindFaceGroupFromVertex(vtHd).size() && YSTRUE!=IsVertexUsedByNonManifoldEdge(vtHd))
			{
				fgVtHd.push_back(vtHd);
			}
		}
	}
	YsRemoveDuplicateInUnorderedArray<VertexHandle>(fgVtHd);
	return fgVtHd;
}

YsShellExt::VolumeHandle YsShellExt::FindVolume(YSHASHKEY fgKey) const
{
	return volumeArray.FindObject(fgKey);
}

