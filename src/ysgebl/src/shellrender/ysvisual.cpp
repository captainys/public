/* ////////////////////////////////////////////////////////////

File Name: ysvisual.cpp
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

#include <ysshelldnmident.h>
#include <ysshellext_orientationutil.h>

#include "ysvisual.h"


void YsVisual::SetPapiColor(const double &angle)
{
	specialRenderingRequirment=RENDER_AS_PAPI;
	papiAngle=angle;
}
void YsVisual::SetVasiColor(const double &angle)
{
	specialRenderingRequirment=RENDER_AS_VASI;
	vasiAngle=angle;
}


////////////////////////////////////////////////////////////

YsVisualSrf::YsVisualSrf(const YsVisualSrf &incoming)
{
	YsShellExt::CopyFrom(incoming);
}
YsVisualSrf &YsVisualSrf::operator=(const YsVisualSrf &incoming)
{
	vboSet.CleanUp();
	YsShellExt::CopyFrom(incoming);
	return *this;
}

void YsVisualSrf::CleanUp(void)
{
	vboSet.CleanUp();
	YsShellExt::CleanUp();
}

YSRESULT YsVisualSrf::Load(YsTextInputStream &inStream)
{
	YsShellExtReader reader;
	CleanUp();
	return reader.MergeSrf(*this,inStream);
}

// Warning!
// Now CPU has total responsibility in calculating the model-view transformation.
// (Because most of the GPUs has such a ****ing low precision.  Give me double!)
// Therefore, this pos and att is relative to the origin.  Not relative to the camera!
void YsVisualSrf::Draw(const YsVec3 &pos,const YsAtt3 &att) const
{
	YsMatrix4x4 tfm;
	tfm.Translate(pos);
	tfm.RotateXZ(att.h());
	tfm.RotateZY(att.p());
	tfm.RotateXY(att.b());
	Draw(tfm,YsVisual::DRAWALL);
}

void YsVisualSrf::Draw(const YsMatrix4x4 &modelView,unsigned int drawFlag) const
{
	if(YSTRUE!=this->IsPolygonVboPrepared())
	{
		this->GetDrawingBuffer().RemakePolygonBuffer(*this,0.8);
		this->RemakePolygonVbo(this->vboSet,this->GetDrawingBuffer());
	}

	YsHasShellExtVboSet::RenderingOption opt;

	if(0!=(YsVisual::DRAWOPAQUE&drawFlag))
	{
		opt.solidPolygon=YSTRUE;
		opt.transparentPolygon=YSFALSE;
		opt.light=YSFALSE;
		this->Render(modelView,opt);
	}
	if(0!=(YsVisual::DRAWTRANSPARENT&drawFlag))
	{
		opt.solidPolygon=YSFALSE;
		opt.transparentPolygon=YSTRUE;
		opt.light=YSTRUE;
		this->Render(modelView,opt);
	}
}
void YsVisualSrf::Draw(
	    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	    const YsVec3 &pos,const YsAtt3 &att,unsigned int drawFlag) const
{
	YsMatrix4x4 tfm=viewTfm;
	tfm.Translate(pos);
	tfm.RotateXZ(att.h());
	tfm.RotateZY(att.p());
	tfm.RotateXY(att.b());

	YsVec3 bbx[2];
	this->GetBoundingBox(bbx);
	if(YSTRUE==YsIsBoundingBoxVisible(bbx,tfm,projTfm))
	{
		if(YSTRUE!=this->IsPolygonVboPrepared())
		{
			this->GetDrawingBuffer().RemakePolygonBuffer(*this,0.8);
			this->RemakePolygonVbo(this->vboSet,this->GetDrawingBuffer());
		}

		YsHasShellExtVboSet::RenderingOption opt;
		opt.TurnOffAll();
		if(0!=(YsVisual::DRAWOPAQUE&drawFlag))
		{
			opt.solidPolygon=YSTRUE;
		}
		if(0!=(YsVisual::DRAWTRANSPARENT&drawFlag))
		{
			opt.transparentPolygon=YSTRUE;
			opt.light=YSTRUE;
		}
		this->Render(tfm,opt);
	}
}

void YsVisualSrf::DrawShadow(
	    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	    const YsVec3 &pos,const YsAtt3 &att,const YsMatrix4x4 &projPlnTfm) const
{
	if(YSTRUE!=this->IsPlainPolygonVboPrepared())
	{
		this->GetDrawingBuffer().RemakePolygonBuffer(*this,0.8);
		this->RemakePlainPolygonVbo(this->vboSet,this->GetDrawingBuffer());
	}

	YsMatrix4x4 tfm=projPlnTfm;
	tfm.Translate(pos);
	tfm.RotateXZ(att.h());
	tfm.RotateZY(att.p());
	tfm.RotateXY(att.b());

	const auto viewModelTfm=viewTfm*tfm;

	YsVec3 bbx[2];
	this->GetBoundingBox(bbx);
	if(YSTRUE==YsIsBoundingBoxVisible(bbx,viewModelTfm,projTfm))
	{
		YsHasShellExtVboSet::RenderingOption opt;
		opt.TurnOffAll();
		opt.shadow=YSTRUE;
		this->Render(viewModelTfm,opt);
	}
}



////////////////////////////////////////////////////////////


YsVisualDnm::YsVisualDnm()
{
	specialRenderingRequirment=RENDER_NORMAL;
	papiAngle=0.0;
}
YsVisualDnm::YsVisualDnm(std::nullptr_t)
{
	specialRenderingRequirment=RENDER_NORMAL;
	papiAngle=0.0;
}
YsVisualDnm::~YsVisualDnm()
{
}
void YsVisualDnm::CacheTransformation(void)
{
	if(dnmPtr)
	{
		dnmPtr->CacheTransformation(dnmState);
	}
}
YSRESULT YsVisualDnm::Load(YsTextInputStream &inStream,const char fn[])
{
	std::shared_ptr <Dnm> dnm(new Dnm);

	auto res=dnm->LoadDnm(inStream,nullptr,fn);
	for(auto nodePtr : dnm->GetNodePointerAll())
	{
		YsShellExt_OrientationUtil orientationUtil;
		orientationUtil.FixOrientationBasedOnAssignedNormal(*nodePtr);
		for(auto plHd : orientationUtil.GetPolygonNeedFlip())
		{
			auto plVtHd=nodePtr->GetPolygonVertex(plHd);
			plVtHd.Invert();
			nodePtr->SetPolygonVertex(plHd,plVtHd);
		}

		// Done just before drawn.
		// auto &buf=nodePtr->GetDrawingBuffer();
		// auto &vboSet=nodePtr->GetVboSet();
		// nodePtr->RemakePolygonBuffer(buf,vboSet,0.8);
		// nodePtr->RemakePlainPolygonVbo(vboSet,buf);
	}

	dnmPtr.swap(dnm);
	return YSOK;
}

void YsVisualDnm::CleanUp(void)
{
	if(dnmPtr)
	{
		dnmPtr->CleanUp();
		dnmPtr.reset();
	}
}

void YsVisualDnm::GetBoundingBox(YsVec3 bbx[2]) const
{
	if(dnmPtr)
	{
		dnmPtr->GetBoundingBox(bbx);
	}
	else
	{
		bbx[0]=YsOrigin();
		bbx[1]=YsOrigin();
	}
}

void YsVisualDnm::GetBoundingBox(YsVec3 &bbx0,YsVec3 &bbx1) const
{
	if(dnmPtr)
	{
		dnmPtr->GetBoundingBox(bbx0,bbx1);
	}
	else
	{
		bbx0=YsOrigin();
		bbx1=YsOrigin();
	}
}

void YsVisualDnm::SetState(int partId,int sta)
{
	SetState(partId,sta,sta,0.0);
}

void YsVisualDnm::ClearState(void)
{
	if(dnmPtr)
	{
		dnmState=dnmPtr->EmptyDnmState();
	}
}

void YsVisualDnm::SetZeroTransformation(void)
{
	if(dnmPtr)
	{
		dnmPtr->SetZeroTransformation(dnmState);
	}
}

void YsVisualDnm::SetStateOfAllPart(int sta)
{
	if(dnmPtr)
	{
		dnmPtr->SetState(dnmState,sta);
	}
}
void YsVisualDnm::SetState(int partId,int st1,int st2,const double &t)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassState(dnmState,partId,st1,st2,t);
	}
}
void YsVisualDnm::SetShow(int partId,YSBOOL show)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassShow(dnmState,partId,show);
	}
}
void YsVisualDnm::SetAngle(int partId,const double &angle)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassHeading(dnmState,partId,angle);
	}
}
void YsVisualDnm::SetRotation(int partId,const double &angle)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassRotation(dnmState,partId,angle);
	}
}
void YsVisualDnm::SetHeading(int partId,const double &angle)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassHeading(dnmState,partId,angle);
	}
}
void YsVisualDnm::SetPitch(int partId,const double &angle)
{
	if(dnmPtr)
	{
		dnmPtr->SetClassPitch(dnmState,partId,angle);
	}
}

 void YsVisualDnm::SetAngleByDnmNodeNumber(int nodeNumber,const double &hdg,const double &pch)
{
	if(nullptr!=dnmPtr)
	{
		auto node=dnmPtr->FindNode((YSHASHKEY)nodeNumber);
		if(nullptr!=node)
		{
			dnmPtr->SetNodeAngle(dnmState,node,hdg,pch);
		}
	}
}

void YsVisualDnm::SetUpSpecialRenderingRequirement(void)
{
	switch(specialRenderingRequirment)
	{
	default:
		break;
	case RENDER_AS_VASI:
		if(dnmPtr && nullptr!=dnmPtr->GetFirstRootNode())
		{
			//                2.5 deg          3.5deg
			// t              0                1.0
			//                Red              White

			const double t=(YsRadToDeg(vasiAngle)-2.5);
			const double gb=YsBound(t,0.0,1.0);

			YsColor col;
			col.SetDoubleRGB(1.0,gb,gb);

			auto &shl=*(dnmPtr->GetFirstRootNode());

			for(auto plHd : shl.AllPolygon())
			{
				shl.SetPolygonColor(plHd,col);
			}

			shl.GetDrawingBuffer().RemakePolygonBuffer(shl,0.8);
			shl.GetDrawingBuffer().AddPolygonAsLight(shl,10.0);
			shl.RemakePolygonVbo(shl.GetVboSet(),shl.GetDrawingBuffer());
		}
		break;
	case RENDER_AS_PAPI:
		if(dnmPtr && nullptr!=dnmPtr->GetFirstRootNode())
		{
			// On course
			// White White Red Red  (RUNWAY)  Red Red White White

			//                1.5 deg          3 deg             4.5 deg
			// t              0                0.5               1.0
			// Light [0]      Red     Red      Red      Red      White       (0.75 to 1.0)
			// Light [1]      Red     Red      Red      White    White       (0.5  to 0.75)
			// Light [2]      Red     Red      White    White    White       (0.25 to 0.5)
			// Light [3]      Red     White    White    White    White       (0.0  to 0.25)

			YsColor col[4];
			const double t=(YsRadToDeg(papiAngle)-1.5)/3.0;

			for(int i=0; i<4; i++)
			{
				const double t0=0.75-0.25*(double)i;
				const double gb=YsBound((t-t0)/0.25,0.0,1.0);
				col[i].SetDoubleRGB(1.0,gb,gb);
			}

			auto &shl=*(dnmPtr->GetFirstRootNode());

			int plId=0;
			for(auto plHd : shl.AllPolygon())
			{
				shl.SetPolygonColor(plHd,col[4*plId/shl.GetNumPolygon()]);
				plId++;
			}

			shl.GetDrawingBuffer().RemakePolygonBuffer(shl,0.8);
			shl.GetDrawingBuffer().AddPolygonAsLight(shl,10.0);
			shl.RemakePolygonVbo(shl.GetVboSet(),shl.GetDrawingBuffer());
		}
		break;
	}
}

// Warning!
// Now CPU has total responsibility in calculating the model-view transformation.
// (Because most of the GPUs has such a ****ing low precision.  Give me double!)
// Therefore, this pos and att is relative to the origin.  Not relative to the camera!
void YsVisualDnm::Draw(const YsVec3 &pos,const YsAtt3 &att) const
{
	YsMatrix4x4 tfm;
	tfm.Translate(pos);
	tfm.RotateXZ(att.h());
	tfm.RotateZY(att.p());
	tfm.RotateXY(att.b());
	Draw(tfm,DRAWALL);
}

void YsVisualDnm::Draw(const YsMatrix4x4 &modelView,unsigned int drawFlag) const
{
	if(dnmPtr)
	{
		auto &dnm=*dnmPtr;
		auto root=dnm.GetRootNodeArray();
		for(auto nodePtr : root)
		{
			Draw(modelView,nodePtr,drawFlag);
		}
	}
}

void YsVisualDnm::Draw(const YsMatrix4x4 &tfm,Dnm::Node *nodePtr,unsigned int drawFlag) const
{
	auto &dnm=*dnmPtr;
	auto &nodeState=dnmState.GetState(nodePtr);
	const YsMatrix4x4 newTfm=tfm*nodeState.tfmCache;

	if(YSTRUE==nodeState.GetShow())
	{
		if(YSTRUE!=nodePtr->IsPolygonVboPrepared())
		{
			nodePtr->GetDrawingBuffer().RemakePolygonBuffer(*nodePtr,0.8);
			nodePtr->RemakePolygonVbo(nodePtr->GetVboSet(),nodePtr->GetDrawingBuffer());
		}

		YsHasShellExtVboSet::RenderingOption opt;
		if(0!=(YsVisual::DRAWOPAQUE&drawFlag))
		{
			opt.solidPolygon=YSTRUE;
			opt.transparentPolygon=YSFALSE;
			opt.light=YSFALSE;
			nodePtr->Render(newTfm,opt);
		}

		if(0!=(YsVisual::DRAWTRANSPARENT&drawFlag))
		{
			opt.solidPolygon=YSFALSE;
			opt.transparentPolygon=YSTRUE;
			opt.light=YSTRUE;
			nodePtr->Render(newTfm,opt);
		}
	}

	for(auto childPtr : nodePtr->children)
	{
		Draw(newTfm,childPtr,drawFlag);
	}
}

 void YsVisualDnm::Draw(
    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
    const YsVec3 &pos,const YsAtt3 &att,unsigned int drawFlag) const
{
	if(dnmPtr)
	{
		auto &dnm=*dnmPtr;

		YsMatrix4x4 tfm;
		tfm.Translate(pos);
		tfm.RotateXZ(att.h());
		tfm.RotateZY(att.p());
		tfm.RotateXY(att.b());

		auto root=dnm.GetRootNodeArray();
		for(auto nodePtr : root)
		{
			Draw(tfm,nodePtr,viewTfm,projTfm,drawFlag);
		}
	}
}
void YsVisualDnm::Draw(
    const YsMatrix4x4 &curTfm,Dnm::Node *nodePtr,
    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
    unsigned int drawFlag) const
{
	auto &dnm=*dnmPtr;
	auto &nodeState=dnmState.GetState(nodePtr);
	const YsMatrix4x4 newTfm=curTfm*nodeState.tfmCache;

	if(YSTRUE==nodeState.GetShow())
	{
		const auto viewModelTfm=viewTfm*newTfm;
		YsVec3 bbx[2];
		nodePtr->GetBoundingBox(bbx);
		if(YSTRUE==YsIsBoundingBoxVisible(bbx,viewModelTfm,projTfm))
		{
			YsHasShellExtVboSet::RenderingOption opt;
			opt.TurnOffAll();
			if(0!=(YsVisual::DRAWOPAQUE&drawFlag))
			{
				opt.solidPolygon=YSTRUE;
			}
			if(0!=(YsVisual::DRAWTRANSPARENT&drawFlag))
			{
				opt.transparentPolygon=YSTRUE;
				opt.light=YSTRUE;
			}

			if(YSTRUE!=nodePtr->IsPolygonVboPrepared())
			{
				nodePtr->GetDrawingBuffer().RemakePolygonBuffer(*nodePtr,0.8);
				nodePtr->RemakePolygonVbo(nodePtr->GetVboSet(),nodePtr->GetDrawingBuffer());
			}

			nodePtr->Render(viewModelTfm,opt);
		}
	}

	for(auto childPtr : nodePtr->children)
	{
		Draw(newTfm,childPtr,viewTfm,projTfm,drawFlag);
	}
}

 void YsVisualDnm::DrawShadow(
	    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	    const YsVec3 &pos,const YsAtt3 &att,const YsMatrix4x4 &projPlnTfm) const
{
	if(dnmPtr)
	{
		auto &dnm=*dnmPtr;

		YsMatrix4x4 modelTfm;
		modelTfm.Translate(pos);
		modelTfm.RotateXZ(att.h());
		modelTfm.RotateZY(att.p());
		modelTfm.RotateXY(att.b());

		auto tfm=projPlnTfm*modelTfm;

		auto root=dnm.GetRootNodeArray();
		for(auto nodePtr : root)
		{
			DrawShadow(tfm,nodePtr,viewTfm,projTfm);
		}
	}
}

void YsVisualDnm::DrawShadow(
    const YsMatrix4x4 &curTfm,Dnm::Node *nodePtr,
    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm) const
{
	auto &dnm=*dnmPtr;
	auto &nodeState=dnmState.GetState(nodePtr);
	const YsMatrix4x4 newTfm=curTfm*nodeState.tfmCache;

	if(YSTRUE==nodeState.GetShow())
	{
		const auto viewModelTfm=viewTfm*newTfm;
		YsVec3 bbx[2];
		nodePtr->GetBoundingBox(bbx);
		if(YSTRUE==YsIsBoundingBoxVisible(bbx,viewModelTfm,projTfm))
		{
			if(YSTRUE!=nodePtr->IsPlainPolygonVboPrepared())
			{
				nodePtr->GetDrawingBuffer().RemakePolygonBuffer(*nodePtr,0.8);
				nodePtr->RemakePlainPolygonVbo(nodePtr->GetVboSet(),nodePtr->GetDrawingBuffer());
			}

			YsHasShellExtVboSet::RenderingOption opt;
			opt.TurnOffAll();
			opt.shadow=YSTRUE;
			nodePtr->Render(viewModelTfm,opt);
		}
	}

	for(auto childPtr : nodePtr->children)
	{
		DrawShadow(newTfm,childPtr,viewTfm,projTfm);
	}
}


 int YsVisualDnm::GetDnmNodeNumberFromName(const char dnmName[]) const
{
	if(nullptr!=dnmPtr)
	{
		auto todo=dnmPtr->GetRootNodeArray();
		for(;;)
		{
			if(0==todo.GetN())
			{
				break;
			}

			auto node=todo.Last();
			todo.DeleteLast();

			if(0==node->nodeName.STRCMP(dnmName))
			{
				return (int)node->GetSearchKey();
			}

			todo.Add(node->children);
		}
	}
	return (int)YSNULLHASHKEY;
}
